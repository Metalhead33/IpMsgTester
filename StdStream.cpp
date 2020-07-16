/*
 * File:   StdStream.cpp
 * Author: metalhead33
 *
 * Created on 2017. február 23., 17:46
 */

#include "StdStream.hpp"

StdStream::StdStream( ) : chandle( nullptr, fclose ) {
    active = false;
    path = "";
    chandle = nullptr;
}

StdStream::StdStream( StdStream &&mov )
    : path( std::move( mov.path ) ), chandle( std::move( mov.chandle ) ),
      active( mov.active ) {
    mov.active = false;
}

void StdStream::operator=( StdStream &&mov ) {
    path = std::move( mov.path );
    chandle = std::move( mov.chandle );
    active = mov.active;
    mov.active = false;
}

StdStream::StdStream(const std::string &newpath, bool ro , bool text)
	: path( newpath ),
	  chandle( fopen( newpath.c_str( ), (text) ? (( ro ) ? "r" : "a" ) : (( ro ) ? "rb" : "ab" )), fclose ) {
    if ( chandle )
		active = true;
}

StdStream::StdStream(const char *newpath, bool ro, bool text)
	: path( newpath ),
	  chandle( fopen( newpath, (text) ? (( ro ) ? "r" : "a" ) : (( ro ) ? "rb" : "ab" )), fclose ) {
	if ( chandle )
		active = true;
}

StdStream::~StdStream( ) {}
Abstract::byteBuffer StdStream::loadIntoBuffer( ) {
    Abstract::byteBuffer temp;
    temp.resize( size( ) );
    read( temp.data( ), size( ) );
    return temp;
}
std::string StdStream::stringize( ) {
    auto buff = loadIntoBuffer( );
    return std::string( reinterpret_cast< const char * >( buff.data( ) ),
                        buff.size( ) );
}

int64_t StdStream::read( void *data, int64_t size ) {
    if ( !active )
        return -1;
    else
        return int64_t( fread( data, 1, size, chandle.get( ) ) );
}

int64_t StdStream::seek( int64_t position, SeekPos whence ) {
    if ( !active )
        return -1;
    switch ( whence ) {
    case SeekPos::SET:
        return fseek( chandle.get( ), position, SEEK_SET );
        break;
    case SeekPos::CUR:
        return fseek( chandle.get( ), position, SEEK_CUR );
        break;
    case SeekPos::END:
        return fseek( chandle.get( ), position, SEEK_END );
        break;
    }
    return -1;
}

int64_t StdStream::tell( ) {
    if ( !active )
        return -1;
    else
        return (int64_t)ftell( chandle.get( ) );
}

int64_t StdStream::size( ) {
    if ( !active )
        return -1;
    else {
        int64_t pos = (int64_t)ftell( chandle.get( ) );
        fseek( chandle.get( ), 0, SEEK_END );
        int64_t size = (int64_t)ftell( chandle.get( ) );
        fseek( chandle.get( ), pos, SEEK_SET );
        return size;
    }
}

int64_t StdStream::write( const void *data, int64_t size ) {
    if ( !active )
        return -1;
    else
        return (int64_t)fwrite( data, 1, size, chandle.get( ) );
}

std::string StdStream::GetPath( ) { return path; }

bool StdStream::IsActive( ) { return active; }

char StdStream::getc( ) { return fgetc( chandle.get( ) ); }
