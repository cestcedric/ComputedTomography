#ifndef IOUTILITIES_H
#define IOUTILITIES_H

#include <algorithm>
#include <regex>
#include <string>
#include <vector>

#include <Eigen/Dense>


/**
 * \brief StringUtils provides common string utilities for file parsing.
 */

struct StringUtils {
        /* \brief remove whitespace at front and back of string
         * \param[in,out] str - string to be trimmed
         */
        static void trim(std::string& str);

        /* \brief convert string to lower case
         * \param[in,out] str - string to be converted
         */
        static void tolower(std::string& str);

        /* \brief convert string to upper case
         * \param[in,out] str - string to be converted
         */
        static void toupper(std::string& str);

        /* \brief convert string to arbitrary datatype using stringstream
         * \param[in] str - string to be converted
         * \return converted datatype
         */
        template<typename Type>
        static Type parseString(const std::string& str);

        /* \brief convert string to vector of arbitrary data type using stringstream
         * \param[in] str - string to be converted
         * \param[out] vec - vector of converted datatypes
         * \return number of converted datatypes
         */
        template<typename Type>
        static int parseString(const std::string& str, std::vector<Type>& vec);
};



/**
 * \brief ByteOrder supplies helper routines to cope with byte order conversions.
 */

struct ByteOrder {
    /// byte order enum
    enum ByteOrderEnum {
        LOW_BYTE_FIRST,     // little endian (e.g. x86)
        HIGH_BYTE_FIRST     // big endian (e.g. PowerPC)
    };

    /// attempt to determine endianness (works on Mac at least..), default to little endian
    static const ByteOrderEnum DEFAULT =
#if defined(BYTE_ORDER) && defined(BIG_ENDIAN) && BYTE_ORDER == BIG_ENDIAN
            HIGH_BYTE_FIRST;
#else
            LOW_BYTE_FIRST;
#endif

    /* \brief shuffle the bytes of the data pointed to by ptr to switch endianness
     * \tparam _DataType       - data type underlying the pointer
     * \param[in,out] ptr      - pointer to the data
     * \param[in]     elements - number of elements of _DataType
     */
    template<typename _DataType>
    static void shuffleBytes(void* ptr, long elements);
};



/**
 * \brief DataTypes supplies helper enums and routines for type conversions.
 */

struct DataTypes {
    /// data type enum
    enum DataTypeEnum {
        INT8,  UINT8,
        INT16, UINT16,
        INT32, UINT32,
        FLOAT32,
        FLOAT64
    };

    /* \brief return the size in bytes for the specified data type
     * \param[in] dataType - type for which to return the size in bytes
     * \return size of dataType in bytes
     */
    static int getSizeForType(const DataTypeEnum& dataType);

    /* \brief parse data of type dataType from stream in into an Eigen::Vector
     *        (dispatches to parseData<_DataType, _Scalar> internally)
     * \tparam _Scalar           - scalar type of Eigen::Vector to return
     * \param[in] in             - stream from which to read
     * \param[in] dataType       - dataType of elements to be read from stream
     * \param[in] sizeInElements - number of elements (of type dataType) to read from stream
     * \param[in] shuffleBytes   - flag whether to shuffle bytes around to account for endianness change
     * \return vector of _Scalar containing the read in values
     */
    template<typename _Scalar>
    static Eigen::Matrix<_Scalar, Eigen::Dynamic, 1> parseData(std::istream& in, const DataTypeEnum& dataType, long sizeInElements, bool shuffleBytes);

private:
    /* \brief parse data of type _DataType from stream in into an Eigen::Vector
     * \tparam _DataType         - data type of elements to be read from stream
     * \tparam _Scalar           - scalar type of Eigen::Vector to return
     * \param[in] in             - stream from which to read
     * \param[in] sizeInElements - number of elements (of type _DataType) to read from stream
     * \param[in] shuffleBytes   - flag whether to shuffle bytes around to account for endianness change
     * \return vector of _Scalar containing the read in values
     */
    template<typename _DataType, typename _Scalar>
    static Eigen::Matrix<_Scalar, Eigen::Dynamic, 1> parseData(std::istream& in, long sizeInElements, bool shuffleBytes);
};





// --------- Implementations -----------

inline void StringUtils::trim(std::string& str)
{
    str = std::regex_replace(str, std::regex("^\\s+|\\s+$"), std::string(""));
}


inline void StringUtils::tolower(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}


inline void StringUtils::toupper(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}


template<typename Type>
inline Type StringUtils::parseString(const std::string& str)
{
    Type value;
    std::stringstream convert(str);
    convert >> value;
    if ( convert.fail() )
        throw std::runtime_error("parseString: string conversion failed");
    return value;
}


template<typename Type>
inline int StringUtils::parseString(const std::string& str, std::vector<Type>& vec)
{
    int count = 0;
    Type value;
    std::stringstream convert(str);
    while (!convert.eof())
    {
        convert >> value;
        if (convert.fail())
            throw std::runtime_error("parseString: string conversion failed");
        vec.push_back(value);
        count++;
    }
    return count;
}


template<>
inline void ByteOrder::shuffleBytes<uint8_t>(void* ptr, long elements) {}

template<>
inline void ByteOrder::shuffleBytes<int8_t>(void* ptr, long elements) {}

template<>
inline void ByteOrder::shuffleBytes<uint16_t>(void* ptr, long elements)
{
    uint16_t* ptr16 = static_cast<uint16_t*>(ptr);
    for (long index = 0; index < elements; ++index)
        ptr16[index] = ((ptr16[index] & 0xff) << 8) | ((ptr16[index] & 0xff00) >> 8);
}

template<>
inline void ByteOrder::shuffleBytes<int16_t>(void* ptr, long elements)
{
    shuffleBytes<uint16_t>(ptr, elements);
}

template<>
inline void ByteOrder::shuffleBytes<uint32_t>(void* ptr, long elements)
{
    uint32_t* ptr32 = static_cast<uint32_t*>(ptr);
    for (long index = 0; index < elements; ++index)
#ifdef _MSC_VER // dumb version for stupid compilers
        ptr32[index] = ((ptr32[index] & 0xff) << 24) | ((ptr32[index] & 0xff00) << 8) | ((ptr32[index] & 0xff0000) >> 8) | ((ptr32[index] & 0xff000000) >> 24);
#else
        ptr32[index] = __builtin_bswap32(ptr32[index]);
#endif
}

template<>
inline void ByteOrder::shuffleBytes<int32_t>(void* ptr, long elements)
{
    shuffleBytes<uint32_t>(ptr, elements);
}

template<>
inline void ByteOrder::shuffleBytes<float>(void* ptr, long elements)
{
    shuffleBytes<uint32_t>(ptr, elements);
}

template<>
inline void ByteOrder::shuffleBytes<double>(void* ptr, long elements)
{
    uint64_t* ptr64 = static_cast<uint64_t*>(ptr);
    for (long index = 0; index < elements; ++index)
#ifdef _MSC_VER // dumb version for stupid compilers
        ptr64[index] = ((ptr64[index] & 0xffl) << 56l) | ((ptr64[index] & 0xff00l) << 40l) | ((ptr64[index] & 0xff0000l) << 24l) | ((ptr64[index] & 0xff000000l) << 8l) | ((ptr64[index] & 0xff00000000l) >> 8l) | ((ptr64[index] & 0xff0000000000l) >> 24l) | ((ptr64[index] & 0xff000000000000l) << 40l) | ((ptr64[index] & 0xff00000000000000l) >> 56l);
#else
        ptr64[index] = __builtin_bswap64(ptr64[index]);
#endif

}


inline int DataTypes::getSizeForType(const DataTypeEnum& dataType)
{
    switch (dataType) {
    case INT8:
    case UINT8:
        return 1;

    case INT16:
    case UINT16:
        return 2;

    case INT32:
    case UINT32:
    case FLOAT32:
        return 4;

    case FLOAT64:
        return 8;

    default:
        throw std::invalid_argument("getSizeForType: unsupported data type.");
    }
}


template<typename _Scalar>
inline Eigen::Matrix<_Scalar, Eigen::Dynamic, 1> DataTypes::parseData(std::istream& in, const DataTypeEnum& dataType, long sizeInElements, bool shuffleBytes)
{
    switch (dataType) {
    case UINT8:
        return parseData<uint8_t, _Scalar>(in, sizeInElements, shuffleBytes);
    case INT8:
        return parseData<int8_t, _Scalar>(in, sizeInElements, shuffleBytes);
    case UINT16:
        return parseData<uint16_t, _Scalar>(in, sizeInElements, shuffleBytes);
    case INT16:
        return parseData<int16_t, _Scalar>(in, sizeInElements, shuffleBytes);
    case UINT32:
        return parseData<uint32_t, _Scalar>(in, sizeInElements, shuffleBytes);
    case INT32:
        return parseData<int32_t, _Scalar>(in, sizeInElements, shuffleBytes);
    case FLOAT32:
        return parseData<float, _Scalar>(in, sizeInElements, shuffleBytes);
    case FLOAT64:
        return parseData<double, _Scalar>(in, sizeInElements, shuffleBytes);
    default:
        throw std::runtime_error("parseData: unsupported data type.");
    }
}


template<typename _DataType, typename _Scalar>
inline Eigen::Matrix<_Scalar, Eigen::Dynamic, 1> DataTypes::parseData(std::istream& in, long sizeInElements, bool shuffleBytes)
{
    long sizeInBytes = sizeInElements * sizeof(_DataType);

    // parse data into temporary buffer
    std::vector<_DataType> tempBuffer(sizeInElements);
    in.read(reinterpret_cast<char*>(&tempBuffer[0]), sizeInBytes);
    if (in.gcount() != sizeInBytes)
        throw std::runtime_error("parseData: failed to read the expected amount of data.");

    // perform byte shuffling to correct endianness
    if (shuffleBytes)
        ByteOrder::shuffleBytes<_DataType>(&tempBuffer[0], sizeInElements);

    // create result vector
    Eigen::Matrix<_Scalar, Eigen::Dynamic, 1> result(sizeInElements);
    for (long i = 0; i < sizeInElements; ++i)
        result(i) = static_cast<_Scalar>(tempBuffer[i]);

    return result;
}



#endif // IOUTILITIES_H
