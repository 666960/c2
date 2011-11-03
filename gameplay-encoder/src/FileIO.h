#ifndef FILEIO_H_
#define FILEIO_H_

#include <iostream>
#include <list>
#include <vector>

#include "Base.h"

namespace gameplay
{

/**
 * Writes an XML element to the specified file stream.
 * @param file Pointer to a FILE object that identifies the stream.
 * @param elementName Name of the XML element to write.
 * @param value Value to write.
 */
void fprintfElement(FILE* file, const char* elementName, float value);
void fprintfElement(FILE* file, const char* elementName, unsigned int value);
void fprintfElement(FILE* file, const char* elementName, unsigned char value);
void fprintfElement(FILE* file, const char* elementName, const char* value);
void fprintfElement(FILE* file, const char* elementName, const std::string& value);
void fprintfElement(FILE* file, const char* elementName, const float values[], int length);

template <class T>
void fprintfElement(FILE* file, const char* format, const char* elementName, std::vector<T> list)
{
    fprintf(file, "<%s>", elementName);
    std::vector<T>::const_iterator i;
    for (i = list.begin(); i != list.end(); i++)
    {
        fprintf(file, format, *i);
    }
    fprintf(file, "</%s>\n", elementName);
}

template <class T>
void fprintfElement(FILE* file, const char* format, const char* elementName, std::list<T> list)
{
    fprintf(file, "<%s>", elementName);
    std::list<T>::const_iterator i;
    for (i = list.begin(); i != list.end(); i++)
    {
        fprintf(file, format, *i);
    }
    fprintf(file, "</%s>\n", elementName);
}

void fprintfMatrix4f(FILE* file, const float* m);

/**
 * Writes binary data to the given file stream.
 * @param value The value to be written
 * @param file The binary file stream.
 */
void write(unsigned char value, FILE* file);
void write(char value, FILE* file);
void write(const char* str, FILE* file);
void write(unsigned int value, FILE* file);
void write(unsigned long value, FILE* file);
void write(unsigned short value, FILE* file);
void write(bool value, FILE* file);
void write(float value, FILE* file);
void write(const float* values, int length, FILE* file);
/**
 * Writes the length of the string and the string bytes to the binary file stream.
 */
void write(const std::string& str, FILE* file);

void writeZero(FILE* file);

/**
 * Writes the length of the list and writes each element value to the binary file stream.
 * @param list The list to write.
 * @param file The binary file stream.
 */
template <class T>
void write(std::list<T> list, FILE* file)
{
    // First write the size of the list
    write(list.size(), file);
    // Then write each element
    std::list<T>::const_iterator i;
    for (i = list.begin(); i != list.end(); i++)
    {
        write(*i, file);
    }
}

/**
 * Writes the length of the vector and writes each element value to the binary file stream.
 * @param vector The vector to write.
 * @param file The binary file stream.
 */
template <class T>
void write(std::vector<T> vector, FILE* file)
{
    // First write the size of the vector
    write(vector.size(), file);
    // Then write each element
    std::vector<T>::const_iterator i;
    for (i = vector.begin(); i != vector.end(); i++)
    {
        write(*i, file);
    }
}


/**
 * Skips over the string at the current file stream offset by moving the file position.
 * Assumes the current position points to the unsigned int length of the string.
 * The string is assumed to be a char array.
 * @param The file stream.
 */
void skipString(FILE* file);

void skipUint(FILE* file);

}
#endif

