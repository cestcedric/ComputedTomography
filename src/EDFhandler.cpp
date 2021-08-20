#include "EDFhandler.h"

#include <fstream>
#include <map>
#include <exception>

#include "IOutilities.h"


Volume EDFHandler::read(const std::string& filename)
{
    // open the file
    std::ifstream file(filename, std::ios::binary);
    if (!file.good())
        throw std::invalid_argument("EDFHandler::read: error opening " + filename + " for reading");

    // read a single character and make sure that a header is opened
    if (file.eof() || file.get() != '{')
        throw std::invalid_argument("EDFHandler::read: failed reading header opening marker");

    std::map<std::string, std::string> attributes;

    // --- read header data
    while (!file.eof()) {
        // skip whitespace
        while (!file.eof()) {
            const int chr = file.peek();
            if (chr != '\r' && chr != '\n' && chr != ' ' && chr != '\t')
                break;
            file.ignore(1);
        }

        // abort if the header is closed
        if (file.eof() || file.peek() == '}')
            break;

        // extract the attribute assignment
        bool quotesSingle = false, quotesDouble = false;
        std::string assignment = "";
        while (!file.eof()) {
            const int chr = file.get();

            // abort on end-of-assignment
            if (chr == ';' && !(quotesSingle || quotesDouble))
                break;

            // check for quote characters
            if (chr == '\'') quotesSingle = !quotesSingle;
            if (chr == '\"') quotesDouble = !quotesDouble;

            assignment += chr;
        }

        // split the assignment, and remove quotes (if they exist)
        auto delim = assignment.find('=');
        if (delim == std::string::npos)
            throw std::invalid_argument("EDFHandler::read: failed reading name/value delimiter");
        std::string name = assignment.substr(0, delim);
        StringUtils::trim(name);
        std::string value = assignment.substr(delim+1);
        StringUtils::trim(value);
        if (value[0] == value[value.size()-1] && (value[0] == '\'' || value[0] == '\"'))
            value = value.substr(1, value.size() - 2);

        StringUtils::tolower(name);
        attributes[name] = value;
    }
    file.ignore(2); // end of header marker

    // --- now parse the header data

    // read the dimensions
    std::vector<int> dim;
    for (int i = 1; ; ++i) {
        // assemble the attribute name
        std::stringstream aux;
        aux << "dim_" << i;

        // try to find the attribute
        auto dimIt = attributes.find(aux.str());
        if (dimIt == attributes.end())
            break;

        dim.push_back(StringUtils::parseString<int>(dimIt->second));
    }
    const int nDims = dim.size();
    if (!nDims)
        throw std::runtime_error("EDFHandler::read: dimension information not found");

    // parse the (non-standard) spacing tag
    std::vector<float> spacing;
    auto spacingIt = attributes.find("spacing");
    if (spacingIt != attributes.end())
        StringUtils::parseString<float>(spacingIt->second, spacing);

    // check for a byte order tag, but fall back to the default value
    ByteOrder::ByteOrderEnum byteorder = ByteOrder::DEFAULT;
    auto byteorderIt = attributes.find("byteorder");
    if (byteorderIt != attributes.end()) {
        std::string byteorderValue = byteorderIt->second;
        StringUtils::tolower(byteorderValue);

        if (byteorderValue == "highbytefirst")
            byteorder = ByteOrder::HIGH_BYTE_FIRST;
        else if (byteorderValue == "lowbytefirst")
            byteorder = ByteOrder::LOW_BYTE_FIRST;
        else
            throw std::runtime_error("invalid byte order value");
    }

    // check for the 'element type' value
    DataTypes::DataTypeEnum datatype;
    auto datatypeIt = attributes.find("datatype");
    if (datatypeIt != attributes.end()) {
        std::string datatypeValue = datatypeIt->second;
        StringUtils::tolower(datatypeValue);

        if (datatypeValue == "signedbyte")
            datatype = DataTypes::INT8;
        else if (datatypeValue == "unsignedbyte")
            datatype = DataTypes::UINT8;
        else if (datatypeValue == "signedshort")
            datatype = DataTypes::INT16;
        else if (datatypeValue == "unsignedshort")
            datatype = DataTypes::UINT16;
        else if (datatypeValue == "float" || datatypeValue == "floatvalue" || datatypeValue == "real")
            datatype = DataTypes::FLOAT32;
        else if (datatypeValue == "double" || datatypeValue == "doublevalue")
            datatype = DataTypes::FLOAT64;
        // todo: add further data types
        else
            throw std::runtime_error("EDFHandler::read: invalid/unsupported data type");
    }
    else
        throw std::runtime_error("EDFHandler::read: data type not found");

    auto compressionIt = attributes.find("compression");
    if (compressionIt != attributes.end())
        throw std::runtime_error("compression not supported");

    int size = 0;
    auto sizeIt = attributes.find("size");
    if (sizeIt != attributes.end())
        size = StringUtils::parseString<int>(sizeIt->second);

    auto imageIt = attributes.find("image");
    if (imageIt != attributes.end() && StringUtils::parseString<int>(imageIt->second) != 1)
        throw std::runtime_error("EDFHandler::read: image not set to 1");

    // convert size
    Eigen::VectorXi dimSizes(nDims);
    for (int i = 0; i < nDims; ++i)
        dimSizes[i] = dim[i];
    if (dimSizes.prod() * DataTypes::getSizeForType(datatype) != size)
        throw std::runtime_error("EDFHandler::read: size inconsistency");

    // convert spacing
    Eigen::VectorXf dimSpacing = Eigen::VectorXf::Ones(nDims);
    if (spacing.size() > 0) {
        if (spacing.size() != nDims)
            throw std::runtime_error("EDFHandler::read: spacing inconsistency");
        for (int i = 0; i < nDims; ++i)
            dimSpacing[i] = spacing[i];
    }

    // compute the byte shuffling flag
    const bool byteShuffle = (byteorder != ByteOrder::DEFAULT);


    // --- read in the image data
    Eigen::VectorXf data = DataTypes::parseData<float>(file, datatype, dimSizes.prod(), byteShuffle);

    file.close();


    // --- setup the volume
    Eigen::Vector3f lowerLeft = Eigen::Vector3f::Zero();
    Eigen::Vector3f upperRight = lowerLeft + (dimSizes.cast<float>().array() * dimSpacing.array() ).matrix();

    Volume vol(lowerLeft, upperRight, dimSpacing);
    vol.setContent(data);

    return vol;
}


void EDFHandler::write(const std::string& filename, const Volume& vol)
{
    // open the EDF file
    std::ofstream file(filename, std::ios::binary);
    if (!file.good())
        throw std::invalid_argument("EDFHandler::write: cannot open " + filename + " for writing");

    // open the header
    file << "{\n";

    file << "HeaderID = EH:000001:000000:000000;\n";
    file << "Image = " << 1 << ";\n";
    if (ByteOrder::DEFAULT == ByteOrder::HIGH_BYTE_FIRST)
        file << "ByteOrder = HighByteFirst;\n";
    else
        file << "ByteOrder = LowByteFirst;\n";
    file << "DataType = " << TYPE_FLOAT << ";\n";

    // write dimension and size
    for (int i = 0; i < 3; ++i)
        file << "Dim_" << (i+1) << " = " << vol.getNumVoxels()[i] << ";\n";
    file << "Size = " << vol.getNumVoxels().prod() * sizeof(float) << ";\n";

    // write spacing
    file << "Spacing =";
    for (int i = 0; i < 3; ++i)
        file << ' ' << vol.getSpacing()[i];
    file << ";\n";

    // pad the header by adding spaces such that the header ends on a kilobyte boundary
    std::size_t n = 1024;
    while (n < (static_cast<std::size_t>(file.tellp()) + 3))
        n += 1024;
    n -= static_cast<std::size_t>(file.tellp()) + 3;
    while (n > 0) {
        file.put(' ');
        n--;
    }

    // close the header
    file << "\n}\n";

    // write the volume data
    file.write(reinterpret_cast<const char*>(vol.getContent().data()), vol.getNumVoxels().prod() * sizeof(float) );
    file.close();
}

