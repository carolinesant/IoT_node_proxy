#pragma once

#include "record.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <arpa/inet.h>

//Define some constants for the different types of tags in the xml file

#define XML_START_RECORD_TAG 9      //<record>\n

#define XML_SOURCE 11         //  <source="
#define XML_DEST 9            //  <dest="
#define XML_USERN 13          //  <username="
#define XML_ID 7              //  <id="
#define XML_GROUP 10          //  <group="
#define XML_SEM 13            //  <semester=\"

#define XML_ATTR_END 5              //" />\n

#define XML_GRADE 10          //  <grade="
#define XML_GRADE_PHD 8             //PhD" />\n
#define XML_GRADE_MASTER 11         //Master" />\n
#define XML_GRADE_BACH 13           //Bachelor" />\n
#define XML_GRADE_NONE 9            //None" />\n

#define XML_COURSES_START_TAG 12    //  <courses>\n
#define XML_COURSE 24               //    <course="IN----" />\n
#define XML_COURSES_END_TAG 13      //  <courses>\n

#define XML_END_RECORD_TAG 10       //</record>\n

/* This function reads bytes from a buffer, interprets them as an XML
 * representation of a Record, and creates a Record that contains this
 * information.
 *
 * The parameter buffer points to the bytes that can be read.
 *   The buffer can contain information for more than one Record
 *   and incomplete information.
 * The parameter bufSize is the number of bytes that can be read.
 * The parameter bytesRead points to an integer where the actually
 *   processed bytes are stored when the function returns non-NULL.
 *
 * The function returns NULL if the buffer does not contain information
 * to fill a Record completely.
 * If the function returns a Record, it is allocated on the heap and
 * filled with information read from the buffer. The integer pointed
 * to by bytesread contains the number of bytes from the buffer that
 * were used to fill this Record.
 */
Record* XMLtoRecord( char* buffer, int bufSize, int* bytesread );

/* This function reads bytes from a buffer, interprets them as a binary
 * representation of a Record, and creates a Record that contains this
 * information.
 *
 * The parameter buffer points to the bytes that can be read.
 *   The buffer can contain information for more than one Record
 *   and incomplete information.
 * The parameter bufSize is the number of bytes that can be read.
 * The parameter bytesRead points to an integer where the actually
 *   processed bytes are stored when the function returns non-NULL.
 *
 * The function returns NULL if the buffer does not contain information
 * to fill a Record completely.
 * If the function returns a Record, it is allocated on the heap and
 * filled with information read from the buffer. The integer pointed
 * to by bytesread contains the number of bytes from the buffer that
 * were used to fill this Record.
 */
Record* BinaryToRecord( char* buffer, int bufSize, int* bytesread );

