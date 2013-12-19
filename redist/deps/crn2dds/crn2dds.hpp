// crn2dds, based on code by SpartanJ and Evan Parker.
// - rlyeh, public domain

#ifndef CRN2DDS_INCLUDE
#define CRN2DDS_INCLUDE
#include <string>

// returns a pure dxt stream (can be uploaded to a GPU directly)
bool crn2dxt( std::string &out, const void *src, size_t len, unsigned reserved = 0 );
bool crn2dxt( std::string &out, const std::string &in, unsigned reserved = 0 );

// returns a cooked dds header + pure dxt stream (can be saved or processed somewhere else)
bool crn2dds( std::string &out, const void *src, size_t len );
bool crn2dds( std::string &out, const std::string &in );

#endif
