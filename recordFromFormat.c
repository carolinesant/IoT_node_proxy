/*
 * This file implements two functions that read XML and binary information from a buffer,
 * respectively, and return pointers to Record or NULL.
 *
 * *** YOU MUST IMPLEMENT THESE FUNCTIONS ***
 *
 * The parameters and return values of the existing functions must not be changed.
 * You can add function, definition etc. as required.
 */
#include "recordFromFormat.h"

Record* XMLtoRecord( char* buffer, int bufSize, int* bytesread )
{
    Record * newRec = newRecord();
    char * restBuf = buffer;

    //Read potensial record start tag
    int bytesNeededNext = XML_START_RECORD_TAG;
    if (*bytesread + bytesNeededNext <= bufSize) {
        if (strncmp(restBuf, "<record>\n", XML_START_RECORD_TAG) != 0) {
            deleteRecord(newRec); 
            return NULL;
        }
        *bytesread += XML_START_RECORD_TAG; restBuf += XML_START_RECORD_TAG;
    }
    
    //Read potensial source attribute
    bytesNeededNext = XML_SOURCE + 1 + XML_ATTR_END;
    if (*bytesread + bytesNeededNext <= bufSize && (strncmp(restBuf, "  <source=\"", XML_SOURCE ) == 0)) {
        *bytesread += XML_SOURCE; restBuf += XML_SOURCE;
            
        setSource(newRec, *restBuf);
        *bytesread += 1; restBuf += 1;
            
        if (strncmp(restBuf, "\" />\n", XML_ATTR_END) == 0) {
            *bytesread += XML_ATTR_END; restBuf += XML_ATTR_END;
        } else {
            deleteRecord(newRec); return NULL;
        }    
    }  

    //Read potensial destination attribute
    bytesNeededNext = XML_DEST + 1 + XML_ATTR_END;
    if (*bytesread + bytesNeededNext <= bufSize && (strncmp(restBuf, "  <dest=\"", XML_DEST ) == 0)) {
        *bytesread += XML_DEST; restBuf += XML_DEST;
            
        setDest(newRec, *restBuf);
        *bytesread += 1; restBuf += 1;
            
        if (strncmp(restBuf, "\" />\n", XML_ATTR_END ) == 0) {
            *bytesread += XML_ATTR_END; restBuf += XML_ATTR_END;
        } else {
             deleteRecord(newRec); return NULL;
        }    
    } 
    
    //Read potensial username attribute
    bytesNeededNext = XML_USERN;
    if (*bytesread + bytesNeededNext <= bufSize && (strncmp(restBuf, "  <username=\"", XML_USERN ) == 0)) {
        *bytesread += 13; restBuf += 13;
            
        char * usernameE = strchr(restBuf, '"');
        if (usernameE == NULL) {
            deleteRecord(newRec);
            return NULL;
        }
        int usernameLen = usernameE - restBuf;
        
        char username[usernameLen + 1];
        strncpy(username,restBuf,usernameLen);
        username[usernameLen] = '\0';

        setUsername(newRec, username);
        *bytesread += usernameLen; restBuf += usernameLen; 

        if (*bytesread + 5 <= bufSize && strncmp(restBuf, "\" />\n", 5 ) == 0) {
            *bytesread += 5; restBuf += 5;
        } else {
            deleteRecord(newRec); return NULL;
        }
    }
    
    //Read potensial id attribute
    bytesNeededNext = XML_ID;
    if (*bytesread + bytesNeededNext <= bufSize && (strncmp(restBuf, "  <id=\"", XML_ID ) == 0)) {
        *bytesread += XML_ID; restBuf += XML_ID;

        uint32_t id = atoi(restBuf);
        setId(newRec, id);
        while(isdigit(*restBuf)) {restBuf++; *bytesread += 1 ;}
        
        if (*bytesread + XML_ATTR_END <= bufSize && strncmp(restBuf, "\" />\n", XML_ATTR_END ) == 0) {
            *bytesread += XML_ATTR_END; restBuf += XML_ATTR_END;
        } else {
            deleteRecord(newRec); return NULL;
        }
    }  

    //Read potensial groupe attribute
    bytesNeededNext = XML_GROUP;
    if (*bytesread + bytesNeededNext <= bufSize && (strncmp(restBuf, "  <group=\"", XML_GROUP ) == 0)) {
        *bytesread += XML_GROUP; restBuf += XML_GROUP;
        
        uint32_t group = atoi(restBuf);
        setGroup(newRec, group);
        while(isdigit(*restBuf)) {restBuf++; *bytesread += 1 ;}
        
        if (*bytesread + XML_ATTR_END <= bufSize && strncmp(restBuf, "\" />\n", XML_ATTR_END ) == 0) {
            *bytesread += XML_ATTR_END; restBuf += XML_ATTR_END;
        } else {
            deleteRecord(newRec); return NULL;
        }
    }  

    //Read potensial semester attribute
    bytesNeededNext = XML_SEM;
    if (*bytesread + bytesNeededNext <= bufSize && (strncmp(restBuf, "  <semester=\"", XML_SEM ) == 0)) {
        *bytesread += XML_SEM; restBuf += XML_SEM;
        
        int8_t sem = atoi(restBuf);
        setSemester(newRec, sem);
        while(isdigit(*restBuf)) {restBuf++; *bytesread += 1;}
        
        if (*bytesread + XML_ATTR_END <= bufSize && strncmp(restBuf, "\" />\n", XML_ATTR_END ) == 0) {
            *bytesread += XML_ATTR_END; restBuf += XML_ATTR_END;
        } else {
            deleteRecord(newRec); return NULL;
        }
    }  
    
    //Read potensial grade attribute
    bytesNeededNext = XML_GRADE;
    if (*bytesread + bytesNeededNext <= bufSize && (strncmp(restBuf, "  <grade=\"", XML_GRADE) == 0)) {
        *bytesread += XML_GRADE; restBuf += XML_GRADE;
        
        Grade grade;
        if (*bytesread + XML_GRADE_PHD <= bufSize && (strncmp(restBuf, "PhD\" />\n", XML_GRADE_PHD) == 0)) {
            grade = Grade_PhD;
            *bytesread += XML_GRADE_PHD; restBuf += XML_GRADE_PHD;
        }
        else if (*bytesread + XML_GRADE_MASTER <= bufSize && (strncmp(restBuf, "Master\" />\n", XML_GRADE_MASTER)) == 0){
            grade = Grade_Master;
            *bytesread += XML_GRADE_MASTER; restBuf += XML_GRADE_MASTER;
                
        }
        else if (*bytesread + XML_GRADE_BACH <= bufSize && (strncmp(restBuf, "Bachelor\" />\n", XML_GRADE_BACH) == 0)) {
            grade = Grade_Bachelor;
            *bytesread += XML_GRADE_BACH; restBuf += XML_GRADE_BACH;
        }
        else if (*bytesread + XML_GRADE_NONE <= bufSize && (strncmp(restBuf, "None\" />\n", XML_GRADE_NONE) == 0)) {
            grade = Grade_None;
            *bytesread += XML_GRADE_NONE; restBuf += XML_GRADE_NONE;
        } else {
            deleteRecord(newRec);
            return NULL;
        }
        setGrade(newRec, grade);
    }  
    
    //Read potensial course attribute
    bytesNeededNext = XML_COURSES_START_TAG;
    if (*bytesread + bytesNeededNext <= bufSize && (strncmp(restBuf, "  <courses>\n", XML_COURSES_START_TAG ) == 0)) {
        *bytesread += XML_COURSES_START_TAG; restBuf += XML_COURSES_START_TAG;
        if (*bytesread + XML_COURSE <= bufSize && strncmp(restBuf, "    <course=\"IN1000\" />\n", XML_COURSE ) == 0) {
            setCourse(newRec, Course_IN1000); *bytesread += XML_COURSE; restBuf += XML_COURSE;
        }
        if (*bytesread + XML_COURSE <= bufSize && strncmp(restBuf, "    <course=\"IN1010\" />\n", XML_COURSE ) == 0) {
            setCourse(newRec, Course_IN1010); *bytesread += XML_COURSE; restBuf += XML_COURSE;
        }
        if (*bytesread + XML_COURSE <= bufSize && strncmp(restBuf, "    <course=\"IN1020\" />\n", XML_COURSE ) == 0) {
            setCourse(newRec, Course_IN1020); *bytesread += XML_COURSE; restBuf += XML_COURSE;
        }
        if (*bytesread + XML_COURSE <= bufSize && strncmp(restBuf, "    <course=\"IN1030\" />\n", XML_COURSE ) == 0) {
            setCourse(newRec, Course_IN1030); *bytesread += XML_COURSE; restBuf += XML_COURSE;
        }
        if (*bytesread + XML_COURSE <= bufSize && strncmp(restBuf, "    <course=\"IN1050\" />\n", XML_COURSE ) == 0) {
            setCourse(newRec, Course_IN1050); *bytesread += XML_COURSE; restBuf += XML_COURSE;
        }
        if (*bytesread + XML_COURSE <= bufSize && strncmp(restBuf, "    <course=\"IN1060\" />\n", XML_COURSE ) == 0) {
            setCourse(newRec, Course_IN1060); *bytesread += XML_COURSE; restBuf += XML_COURSE;
        }
        if (*bytesread + XML_COURSE <= bufSize && strncmp(restBuf, "    <course=\"IN1080\" />\n", XML_COURSE ) == 0) {
            setCourse(newRec, Course_IN1080); *bytesread += XML_COURSE; restBuf += XML_COURSE;
        }
        if (*bytesread + XML_COURSE <= bufSize && strncmp(restBuf, "    <course=\"IN1140\" />\n", XML_COURSE ) == 0) {
            setCourse(newRec, Course_IN1140); *bytesread += XML_COURSE; restBuf += XML_COURSE;
        }
        if (*bytesread + XML_COURSE <= bufSize && strncmp(restBuf, "    <course=\"IN1150\" />\n", XML_COURSE ) == 0) {
            setCourse(newRec, Course_IN1150); *bytesread += XML_COURSE; restBuf += XML_COURSE;
        }
        if (*bytesread + XML_COURSE <= bufSize && strncmp(restBuf, "    <course=\"IN1900\" />\n", XML_COURSE ) == 0) {
            setCourse(newRec, Course_IN1900); *bytesread += XML_COURSE; restBuf += XML_COURSE;
        }
        if (*bytesread + XML_COURSE <= bufSize && strncmp(restBuf, "    <course=\"IN1910\" />\n", XML_COURSE ) == 0) {
            setCourse(newRec, Course_IN1910); *bytesread += XML_COURSE; restBuf += XML_COURSE;
        }
        if (*bytesread + XML_COURSES_END_TAG <= bufSize && strncmp(restBuf, "  </courses>\n", 13 ) == 0) {
            *bytesread += XML_COURSES_END_TAG; restBuf += XML_COURSES_END_TAG;
        } else {
            deleteRecord(newRec);
            return NULL;
        }

    }  
    
    //Read potensial record end tag
    bytesNeededNext = XML_END_RECORD_TAG;
    if (*bytesread + bytesNeededNext <= bufSize && (strncmp(restBuf, "</record>\n", XML_END_RECORD_TAG ) == 0)) {
        *bytesread += XML_END_RECORD_TAG;
        return newRec;
    }  
    
    //If the record has not been returned in the end record tag if statement it is deleted and NULL is returned
    deleteRecord(newRec);
    return NULL;
}


Record* BinaryToRecord( char* buffer, int bufSize, int* bytesread )
{
    Record * newRec = newRecord();
    char * restBuf = buffer;
    if (*bytesread + 1 > bufSize) {deleteRecord(newRec); return NULL;}  
    
    if (*restBuf & FLAG_SRC) {newRec->has_source = true;}
    if (*restBuf & FLAG_DST) {newRec->has_dest = true;}
    if (*restBuf & FLAG_USERNAME) {newRec->has_username = true;}
    if (*restBuf & FLAG_ID) {newRec->has_id = true;}
    if (*restBuf & FLAG_GROUP) {newRec->has_group = true;}
    if (*restBuf & FLAG_SEMESTER) {newRec->has_semester = true;}
    if (*restBuf & FLAG_GRADE) {newRec->has_grade = true;}
    if (*restBuf & FLAG_COURSES) {newRec->has_courses = true;}
    *bytesread += 1; restBuf += 1;

    if (newRec->has_source) {
        if (*bytesread + 1 > bufSize) {deleteRecord(newRec); return NULL;} 
        setSource(newRec, *restBuf); 
        *bytesread += 1; restBuf += 1;
    }
    
    if (newRec->has_dest) {
        if (*bytesread + 1 > bufSize) {deleteRecord(newRec); return NULL;} 
        setDest(newRec, *restBuf); 
        *bytesread += 1; restBuf += 1;
    }

    if (newRec->has_username) {
        if (*bytesread + 4 > bufSize) {deleteRecord(newRec); return NULL;} 
        unsigned int len = 0;
        memcpy(&len, restBuf, 4);
        len = ntohl(len);
        *bytesread += 4; restBuf += 4;
        
        if (*bytesread + len > bufSize) {deleteRecord(newRec); return NULL;} 
        char username[len + 1];
        strncpy(username, restBuf, len);
        username[len] = '\0';
        
        setUsername(newRec, username);
        *bytesread += len; restBuf += len; 
    }

    if (newRec->has_id) {
        if (*bytesread + 4 > bufSize) {deleteRecord(newRec); return NULL;} 
        uint32_t id = 0;
        memcpy(&id, restBuf, 4);
        id = ntohl(id);
        setId(newRec, id);
        *bytesread += 4; restBuf += 4;
    }

    if (newRec->has_group) {
        if (*bytesread + 4 > bufSize) {deleteRecord(newRec); return NULL;} 
        uint32_t group = 0;
        memcpy(&group, restBuf, 4);
        group = ntohl(group);
        setGroup(newRec, group);
        *bytesread += 4; restBuf += 4;
    }

    if (newRec->has_semester) {
        if (*bytesread + 1 > bufSize) {deleteRecord(newRec); return NULL;} 
        uint8_t sem = *restBuf;
        setSemester(newRec, sem);
        *bytesread += 1; restBuf += 1;
    }

    if (newRec->has_grade) {
        if (*bytesread + 1 > bufSize) {deleteRecord(newRec); return NULL;} 
        Grade grade = *restBuf;
        setGrade(newRec, grade);
        *bytesread += 1; restBuf += 1;
    }

    if (newRec->has_courses) {
        if (*bytesread + 2 > bufSize) {deleteRecord(newRec); return NULL;}
        uint16_t courses = 0;
        memcpy(&courses, restBuf, 2);
        courses = ntohs(courses);
        
        setCourse(newRec, courses);
        *bytesread += 2; restBuf += 2;
    }
    return newRec;
}



