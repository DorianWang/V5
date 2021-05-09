/* -*- c++ -*-
 *  $Id: dom_extvar.h 14623 2021-05-08 12:52:15Z greg $
 *
 *  Created by Martin Mroz on 02/03/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __LQIO_DOM_EXTVAR__
#define __LQIO_DOM_EXTVAR__

#include <string>
#include <ostream>
#include <stdexcept>
#include <lqx/SymbolTable.h>
#include <lqx/Program.h>

namespace LQIO {
    class LQXDocument;
    
    namespace DOM {
    
	class ConstantExternalVariable;
	
	class ExternalVariable {
	    friend std::ostream& operator<<( std::ostream&, const LQIO::DOM::ExternalVariable& );

	public:
	    typedef enum { VAR_UNASSIGNED, VAR_DOUBLE, VAR_STRING } Type;
	    
	public:
      
	    /* Designated Initializers for Variables */
	    ExternalVariable();
	    ExternalVariable( const ExternalVariable& );
	    ExternalVariable& operator=( const ExternalVariable& );
	    virtual ~ExternalVariable();
	    static ExternalVariable * clone( const ExternalVariable * );
	    virtual ExternalVariable * clone() const = 0;

	public:
	    virtual Type getType() const { return VAR_UNASSIGNED; }

	    /* Obtaining the Value */
	    virtual void set(double value) = 0;
	    virtual void setString( const char * value ) = 0;
	    virtual bool getValue(double& result) const = 0;
	    virtual bool getString( const char *& result) const = 0;
	    virtual const std::string& getName() const = 0;
	    virtual bool wasSet() const = 0;

	    static bool isPresent( const ExternalVariable *, double lower_limit=0. );	/* Variable is present (may not be instantiated) */
	    static bool isDefault( const ExternalVariable *, double default_value=0. );	/* Variable is present (may not be instantiated) */
	    
	protected:
	    virtual std::ostream& print( std::ostream& ) const = 0;
	    virtual std::ostream& printVariableName( std::ostream& ) const = 0;

	private:
	    static std::ostream& printVariableName( std::ostream&, const ExternalVariable& );
	};
    
	/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
	/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
	/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
    
	class ConstantExternalVariable : public ExternalVariable {
	public:
	    /* Initializing the constant external variable */
	    ConstantExternalVariable(double constant=0);
	    ConstantExternalVariable(const char * constant);
	    ConstantExternalVariable& operator=( const ConstantExternalVariable& );
	    ConstantExternalVariable& operator=( double );
	    ConstantExternalVariable( const ExternalVariable& );
	    virtual ConstantExternalVariable * clone() const;
	    virtual ~ConstantExternalVariable();
      

	public:
	    virtual Type getType() const { return _variableType; }

	    /* Obtaining the Value */
	    virtual void set(double value);
	    virtual void setString( const char * );
	    virtual bool getValue(double& result) const;
	    virtual bool getString( const char *& result) const;
	    virtual const std::string& getName() const;
	    virtual bool wasSet() const;
      
	protected:
	    virtual std::ostream& print( std::ostream& ) const;
	    virtual std::ostream& printVariableName( std::ostream& ) const;

	private:
      
	    /* The stored value */
	    Type _variableType;
	    union {
		double d;
		const char * s;
	    } _value;
	};
    
	/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
	/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
	/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
    
	class SymbolExternalVariable : public ExternalVariable {
	    friend class LQIO::LQXDocument;
	    
	public:
      
	    /* Designated initializers once again */
	    SymbolExternalVariable(const std::string& name);
	    SymbolExternalVariable& operator=( const SymbolExternalVariable& );
	    virtual SymbolExternalVariable * clone() const;
	    virtual ~SymbolExternalVariable();
      
	protected:
	    SymbolExternalVariable( const SymbolExternalVariable& );

	public:
	    virtual Type getType() const;

	    /* Registering in an environment */
	    bool registerInEnvironment(LQX::Program* pgm);
      
	    /* Obtaining the Value */
	    virtual void set(double value);
	    virtual void setString(const char * s);
	    virtual bool getValue(double& result) const;
	    virtual bool getString(const char *& result) const;
	    virtual bool wasSet() const;
	    virtual const std::string& getName() const { return _name; }
      
	protected:
	    virtual std::ostream& print( std::ostream& ) const;
	    virtual std::ostream& printVariableName( std::ostream& ) const;

	    /* This one's a bit more complicated */
	private:
	    LQX::SymbolAutoRef _externalSymbol;
	    std::string _name;
	};
    
	double to_double( const LQIO::DOM::ExternalVariable& );
	double to_unsigned( const LQIO::DOM::ExternalVariable& );
	const char * to_string( const LQIO::DOM::ExternalVariable& );
    }
}

#endif /* __LQIO_DOM_EXTVAR__ */
