/* -*- c++ -*-
 * $Id: expat_document.cpp 13764 2020-08-17 19:50:05Z greg $
 *
 * Read in XML input files.
 *
 * Copyright the Real-Time and Distributed Systems Group,
 * Department of Systems and Computer Engineering,
 * Carleton University, Ottawa, Ontario, Canada. K1S 5B6
 *
 * ------------------------------------------------------------------------
 * December 2020.
 * ------------------------------------------------------------------------
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <fcntl.h>
#include <sys/stat.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_SYS_MMAN_H
#include <sys/types.h>
#include <sys/mman.h>
#endif
#include "expat_document.h"
#include "jmva_document.h"
#include "error.h"
#include "glblerr.h"
#include "input.h"
#include "common_io.h"
#include "xml_input.h"
#include "xml_output.h"


namespace BCMP {
    /* ---------------------------------------------------------------- */
    /* DOM input.                                                       */
    /* ---------------------------------------------------------------- */

    JMVA_Document::JMVA_Document( const std::string& input_file_name ) : _document(new BCMP::JMVA()), _input_file_name(input_file_name), _parser(nullptr), _stack()
    {
    }
    
    /* static */ JMVA_Document *
    JMVA_Document::create( const std::string& input_file_name )
    {
	JMVA_Document * document = new JMVA_Document( input_file_name );
	if ( document->parse() ) {
	    return document;
	} else {
	    delete document;
	    return nullptr;
	}
    }

    bool
    JMVA_Document::parse()
    {
	struct stat statbuf;
	bool rc = true;
	int input_fd = -1;

	if ( _input_file_name ==  "-" ) {
	    input_fd = fileno( stdin );
	} else if ( ( input_fd = open( _input_file_name.c_str(), O_RDONLY ) ) < 0 ) {
	    std::cerr << LQIO::io_vars.lq_toolname << ": Cannot open input file " << _input_file_name << " - " << strerror( errno ) << std::endl;
	    return false;
	}

	if ( isatty( input_fd ) ) {
	    std::cerr << LQIO::io_vars.lq_toolname << ": Input from terminal is not allowed." << std::endl;
	    return false;
	} else if ( fstat( input_fd, &statbuf ) != 0 ) {
	    std::cerr << LQIO::io_vars.lq_toolname << ": Cannot stat " << _input_file_name << " - " << strerror( errno ) << std::endl;
	    return false;
#if defined(S_ISSOCK)
	} else if ( !S_ISREG(statbuf.st_mode) && !S_ISFIFO(statbuf.st_mode) && !S_ISSOCK(statbuf.st_mode) ) {
#else
	} else if ( !S_ISREG(statbuf.st_mode) && !S_ISFIFO(statbuf.st_mode) ) {
#endif
	    std::cerr << LQIO::io_vars.lq_toolname << ": Input from " << _input_file_name << " is not allowed." << std::endl;
	    return false;
	}

	_parser = XML_ParserCreateNS(NULL,'/');     /* Gobble header goop */
	if ( !_parser ) {
	    throw std::runtime_error("Could not allocate memory for Expat.");
	}

	XML_SetElementHandler( _parser, start, end );
	XML_SetCdataSectionHandler( _parser, start_cdata, end_cdata );
	XML_SetCharacterDataHandler( _parser, handle_text );
	XML_SetCommentHandler( _parser, handle_comment );
	XML_SetUnknownEncodingHandler( _parser, handle_encoding, static_cast<void *>(this) );
	XML_SetUserData( _parser, static_cast<void *>(this) );

	_stack.push( parse_stack_t("",&JMVA_Document::startDocument,0) );

#if HAVE_MMAP
	char *buffer;
#endif
	try {
#if HAVE_MMAP
	    buffer = static_cast<char *>(mmap( 0, statbuf.st_size, PROT_READ, MAP_PRIVATE|MAP_FILE, input_fd, 0 ));
	    if ( buffer != MAP_FAILED ) {
		if ( !XML_Parse( _parser, buffer, statbuf.st_size, true ) ) {
		    const char * error = XML_ErrorString(XML_GetErrorCode(_parser));
		    LQIO::input_error( error );
		    rc = false;
		}
	    } else {
		/* Try the old way (for pipes) */
#endif
		const size_t BUFFSIZE = 1024;
		char buffer[BUFFSIZE];
		size_t len = 0;

		do {
		    len = read( input_fd, buffer, BUFFSIZE );
		    if ( static_cast<int>(len) < 0 ) {
			std::cerr << LQIO::io_vars.lq_toolname << ": Read error on " << _input_file_name << " - " << strerror( errno ) << std::endl;
			rc = false;
			break;
		    } else if (!XML_Parse(_parser, buffer, len, len == 0 )) {
			const char * error = XML_ErrorString(XML_GetErrorCode(_parser));
			LQIO::input_error( error );
			rc = false;
			break;
		    }
		} while ( len > 0 );
#if HAVE_MMAP
	    }
#endif
	}
	/* Halt on any error. */
	catch ( const LQIO::element_error& e ) {
	    LQIO::input_error( "Unexpected element <%s> ", e.what() );
	    rc = false;
	}
	catch ( const LQIO::missing_attribute & e ) {
	    rc = false;
	}
	catch ( const LQIO::unexpected_attribute & e ) {
	    rc = false;
	}
	catch ( const std::invalid_argument & e ) {
	    rc = false;
	}
	catch ( const LQIO::undefined_symbol & e ) {
	    rc = false;
	}
	catch ( const std::domain_error & e ) {
	    LQIO::input_error( "Domain error: %s ", e.what() );
	    rc = false;
	}

#if HAVE_MMAP
	if ( buffer != MAP_FAILED ) {
	    munmap( buffer, statbuf.st_size );
	}
#endif
	XML_ParserFree(_parser);
	_parser = 0;
	close( input_fd );
	return rc;
    }

    /*
     * Handlers called from Expat.
     */

    void
    JMVA_Document::start( void *data, const XML_Char *el, const XML_Char **attr )
    {
	JMVA_Document * document = static_cast<JMVA_Document *>(data);
	LQIO_lineno = XML_GetCurrentLineNumber(document->_parser);
	if ( LQIO::DOM::Document::__debugXML ) {
	    std::cerr << std::setw(4) << LQIO_lineno << ": ";
	    for ( unsigned i = 0; i < document->_stack.size(); ++i ) {
		std::cerr << "  ";
	    }
	    std::cerr << "<" << el;
	    for ( const XML_Char ** attributes = attr; *attributes; attributes += 2 ) {
		std::cerr << " " << *attributes << "=\"" << *(attributes+1) << "\"";
	    }
	    std::cerr << ">" << std::endl;
	}
	try {
	    if ( document->_stack.size() > 0 ) {
		const parse_stack_t& top = document->_stack.top();
		(document->*top.start)(top.object,el,attr);
	    }
	}
	catch ( const LQIO::duplicate_symbol& e ) {
	    LQIO::input_error2( LQIO::ERR_DUPLICATE_SYMBOL, el, e.what() );
	}
	catch ( const LQIO::missing_attribute & e ) {
	    LQIO::input_error2( LQIO::ERR_MISSING_ATTRIBUTE, el, e.what() );
	}
	catch ( const LQIO::unexpected_attribute & e ) {
	    LQIO::input_error2( LQIO::ERR_UNEXPECTED_ATTRIBUTE, el, e.what() );
	}
	catch ( const std::invalid_argument & e ) {
	    LQIO::input_error2( LQIO::ERR_INVALID_ARGUMENT, el, e.what() );
	}
	catch ( const LQIO::undefined_symbol & e ) {
	    LQIO::input_error2( LQIO::ERR_NOT_DEFINED, e.what() );
	}
    }

    /*
     * Pop elements off the stack until we hit a matching tag.
     */

    void
    JMVA_Document::end( void *data, const XML_Char *el )
    {
	JMVA_Document * document = static_cast<JMVA_Document *>(data);
	bool done = false;
	while ( document->_stack.size() > 0 && !done ) {
	    parse_stack_t& top = document->_stack.top();
	    if ( LQIO::DOM::Document::__debugXML ) {
		std::cerr << std::setw(4) << LQIO_lineno << ": ";
		for ( unsigned i = 1; i < document->_stack.size(); ++i ) {
		    std::cerr << "  ";
		}
		if ( top.element.size() ) {
		    std::cerr << "</" << top.element << ">" << std::endl;
		} else {
		    std::cerr << "empty stack" << std::endl;
		}
	    }
	    done = top == el;
	    if ( top.end ) {		/* Run the end handler if necessary */
		(document->*top.end)(top.object,el);
	    }
	    document->_stack.pop();
	}
    }

    void
    JMVA_Document::start_cdata( void * data )
    {
    }


    void
    JMVA_Document::end_cdata( void * data )
    {
    }


    /*
     * Ignore most text.  However, for an LQX program, concatenate
     * the text.  Since expat gives us text in "chunks", we can't
     * just simply call setLQXProgram.  Rather, we "append" the
     * program to the existing one.
     */

    void
    JMVA_Document::handle_text( void * data, const XML_Char * text, int length )
    {
	JMVA_Document * parser = static_cast<JMVA_Document *>(data);
	if ( parser->_stack.size() == 0 ) return;
#if 0
	if ( top.start == &JMVA_Document::startLQX ) {
	    std::string& program = const_cast<std::string &>(parser->_document.getLQXProgramText());
	    program.append( text, length );
	} else if ( top.start == &JMVA_Document::startSPEXParameters ) {
	    parser->_spex_parameters.append( text, length );
	} else if ( top.start == &JMVA_Document::startSPEXResults ) {
	    parser->_spex_results.append( text, length );
	} else if ( top.start == &JMVA_Document::startSPEXConvergence ) {
	    parser->_spex_convergence.append( text, length );
	}
#endif
    }

    /*
     * We tack the comment onto the current element.
     */

    void
    JMVA_Document::handle_comment( void * data, const XML_Char * text )
    {
	JMVA_Document * parser = static_cast<JMVA_Document *>(data);
	if ( parser->_stack.size() == 0 ) return;
	const parse_stack_t& top = parser->_stack.top();
	JMVA::BCMP_Object * object = top.object;
	if ( object ) {
	    std::string& comment = const_cast<std::string&>(object->getComment());
	    if ( comment.size() ) {
		comment += "\n";
	    }
	    comment += text;
	}
    }

    int
    JMVA_Document::handle_encoding( void * data, const XML_Char *name, XML_Encoding *info )
    {
	if ( strcasecmp( name, "ascii" ) == 0 ) {
	    /* Initialize the info argument to handle plain old ascii. */
	    for ( unsigned int i = 0; i < 256; ++i ) {
		info->map[i] = i;
	    }
	    info->convert = 0;		/* No need as its all 1 to 1. */
	    info->data = data;		/* The data argument is a pointer to the current document. */
	    return XML_STATUS_OK;
	} else {
	    return XML_STATUS_ERROR;
	}
    }

    bool
    JMVA_Document::parse_stack_t::operator==( const XML_Char * str ) const
    {
	return element == str;
    }

    bool
    JMVA_Document::checkAttributes( const XML_Char * element_name, const XML_Char ** attributes, std::set<const XML_Char *,JMVA_Document::attribute_table_t>& table ) const
    {
	bool rc = true;
	for ( ; *attributes; attributes += 2 ) {
	    std::set<const XML_Char *>::const_iterator item = table.find(*attributes);
	    if ( item == table.end() ) {
		if ( strncasecmp( *attributes, "http:", 5 ) != 0 ) {                /* Skip these */
		    LQIO::input_error2( LQIO::ERR_UNEXPECTED_ATTRIBUTE, element_name, *attributes );
		    rc = false;
		}
	    }
	}
	return rc;
    }
    

    /* ---------------------------------------------------------------- */
    /* Parser functions.                                                */
    /* ---------------------------------------------------------------- */
    void
    JMVA_Document::startDocument( JMVA::BCMP_Object * object, const XML_Char * element, const XML_Char ** attributes )
    {
	checkAttributes( element, attributes, document_table );
	if ( strcasecmp( element, Xmodel ) == 0 ) {
	    LQIO::DOM::Document::__debugXML = (LQIO::DOM::Document::__debugXML || XML::getBoolAttribute(attributes,Xxml_debug));
	    _stack.push( parse_stack_t(element,&JMVA_Document::startModel,nullptr) );
	} else {
	    throw LQIO::element_error( element );
	}
    }

    void
    JMVA_Document::startModel( JMVA::BCMP_Object * object, const XML_Char * element, const XML_Char ** attributes )
    {
	checkAttributes( element, attributes, null_table );
	if ( strcasecmp( element, Xparameters) == 0 ) {
//	    handleModel( object, attributes );
	    _stack.push( parse_stack_t(element,&JMVA_Document::startParameters,0) );
	} else if ( strcasecmp( element, XalgParams) == 0 ) {
	}
    }

    void
    JMVA_Document::startParameters( JMVA::BCMP_Object * object, const XML_Char * element, const XML_Char ** attributes )
    {
	checkAttributes( element, attributes, null_table );
	if ( strcasecmp( element, Xclasses) == 0 ) {
	    _stack.push( parse_stack_t(element,&JMVA_Document::startClasses,nullptr) );
	} else if ( strcasecmp( element, Xstations) == 0 ) {
	    _stack.push( parse_stack_t(element,&JMVA_Document::startStations,nullptr) );
	} else {
	    throw LQIO::element_error( element );
	}
    }

    void
    JMVA_Document::startClasses( JMVA::BCMP_Object * object, const XML_Char * element, const XML_Char ** attributes )
    {
	checkAttributes( element, attributes, parameter_table );
	/* Don't care about number=... */
	if ( strcasecmp( element, Xclosedclass) == 0 ) {
	    _stack.push( parse_stack_t(element,&JMVA_Document::startClass,nullptr) );
	} else if ( strcasecmp( element, Xopenclass) == 0 ) {
	    _stack.push( parse_stack_t(element,&JMVA_Document::startClass,nullptr) );
	} else {
	    throw LQIO::element_error( element );
	}
    }

    void
    JMVA_Document::startClass( JMVA::BCMP_Object * object, const XML_Char * element, const XML_Char ** attributes )
    {
	checkAttributes( element, attributes, class_table );
	if ( strcasecmp( element, Xclosedclass) == 0 ) {
//	    _stack.push( parse_stack_t(element,&JMVA_Document::startClass,new BCMP::Class()) );
	} else if ( strcasecmp( element, Xopenclass) == 0 ) {
//	    _stack.push( parse_stack_t(element,&JMVA_Document::startClass,new BCMP::Class()) );
	} else {
	    throw LQIO::element_error( element );
	}
    }

    void
    JMVA_Document::startStations( JMVA::BCMP_Object * object, const XML_Char * element, const XML_Char ** attributes )
    {
	checkAttributes( element, attributes, parameter_table );
	/* Don't care about number=... */
	throw LQIO::element_error( element );
    }
}

namespace BCMP {
    void
    JMVA_Document::init_tables()
    {
	if ( document_table.size() != 0 ) return;

	document_table.insert(Xxml_debug);
	parameter_table.insert(Xnumber);
	station_table.insert(Xname);
    }

    std::set<const XML_Char *,JMVA_Document::attribute_table_t> JMVA_Document::class_table;
    std::set<const XML_Char *,JMVA_Document::attribute_table_t> JMVA_Document::document_table;
    std::set<const XML_Char *,JMVA_Document::attribute_table_t> JMVA_Document::null_table;
    std::set<const XML_Char *,JMVA_Document::attribute_table_t> JMVA_Document::parameter_table;
    std::set<const XML_Char *,JMVA_Document::attribute_table_t> JMVA_Document::station_table;
    
    const XML_Char * JMVA_Document::XReferenceStation	= "ReferenceStation";
    const XML_Char * JMVA_Document::XalgParams		= "algParams";
    const XML_Char * JMVA_Document::Xclasses		= "classes";
    const XML_Char * JMVA_Document::Xclosedclass	= "closedclass";
    const XML_Char * JMVA_Document::Xcustomerclass	= "customerclass";
    const XML_Char * JMVA_Document::Xdelaystation	= "delaystation";
    const XML_Char * JMVA_Document::Xlistation		= "listation";
    const XML_Char * JMVA_Document::Xmodel		= "model";
    const XML_Char * JMVA_Document::Xname		= "name";
    const XML_Char * JMVA_Document::Xnumber		= "number";
    const XML_Char * JMVA_Document::Xopenclass		= "openclass";
    const XML_Char * JMVA_Document::Xparameters		= "parameters";
    const XML_Char * JMVA_Document::Xservicetimes	= "servicetimes";
    const XML_Char * JMVA_Document::Xstations		= "stations";
    const XML_Char * JMVA_Document::Xvisits		= "visits";
    const XML_Char * JMVA_Document::Xxml_debug 		= "xml-debug";
}