#ifndef EDFHANDLER_H
#define EDFHANDLER_H

#include <string>

#include "volume.h"


/**
 * \brief EDFHandler reads and writes EDF files.
 */

class EDFHandler {
public:
    /* \brief read an EDF file into a Volume
     * \param[in] filename - filename of the EDF file to be read
     * \return volume containing the data
     */
    static Volume read(const std::string& filename);

    /* \brief write out a Volume into an EDF file
     * \param[in] filename - filename of the EDF file to be written
     * \param[in] vol      - volume to be written
     */
    static void write(const std::string& filename, const Volume& vol);

private:
    static constexpr auto TYPE_FLOAT = "FloatValue";
};


#endif // EDFHANDLER_H

