PixInsight Class Library - Coding Style Guidelines
==================================================

Version 0.2 - 2014 November 03<br/>
*Initial release.*

==================================================


### 1. Indentation


   1.1. Use spaces, not tabulators. Tabulators should only appear in files where they play a semantic role, like makefiles.

   1.2. The indentation size is 3 spaces.

      if ( condition )
         DoSomething();      // right

      if ( condition )
              DoSomething(); // wrong

   1.3. The contents of a namespace block should not be indented. Nested namespaces should be indented. Use line comments to mark the closing brace of each namespace.

      namespace pcl
      {

      class Foo
      {
         ...
      };

         namespace nested_namespace
         {

         class Bar
         {
            ...
         };

         } // pcl::nested_namespace

      } // pcl

   1.4. A case label should line up with its switch statement. The case statement must be indented.

      switch ( i )
      {
      case 0:
         DoSomething();
         break;
      case 1:
         DoSomethingElse();
         break;
      default:
         break;
      }

   1.5. A class member access specifier should line up with its class name.

      class Foo
      {
      public:  // right
         ...
      private: // right
         ...
         class Bar
         {
         public:  // right
            ...
         private: // right
            ...
         };
      };

      class Foo
      {
         public:  // wrong
         ...
         private: // wrong
         ...
         class Bar
         {
            public:  // wrong
               ...
            private: // wrong
               ...
         };
      };

   1.6. Try to align and indent line comments to improve readability.

      DoSomething();          // these comments
      x = DoFoo();            // are aligned and indented
      if ( x == 0 )           // for improved...
         PerformBarAction();  //    ...readability

      DoSomething(); // these comments
      x = Foo(); // are not aligned
      if ( x == 0 ) // and hence
         PerformBarAction(); // are much harder to read


### 2. Spacing


   2.1. Do not place spaces around unary operators.

      ++i;  // right
      ++ i; // wrong

   2.2. Place spaces around assignment operators

      x=0;                 // wrong
      x = 0;               // right
      if ( (x = ++i) > 2 ) // right

   2.3. Use spaces to improve readability of binary and ternary operators and numerical expressions.

      x = a + b;     // right
      x = a+b;       // wrong
      y = a + b*c;   // right - spacing improves understanding of operator precedences
      z = (a + b)/c; // right - spacing improves readability of numerical expressions
      x = a ? b : c; // right - ternary operator
      x = a ? b:c;   // wrong

   2.4. Do not place spaces before comma and semicolon.

      delete foo, foo = 0;            // right
      delete foo , foo = 0;           // wrong
      for ( int i = 0; i < 3; ++i )   // right
      for ( int i = 0 ; i < 3 ; ++i ) // wrong
      DoSomething();                  // right
      Dosomething() ;                 // wrong
      CallMeFoo( a, b, c );           // right
      CallMeFoo( a, b , c );          // wrong

   2.5. Place one space before and after colon in base class specifications.

      class Foo : public Bar // right
      {
         ...
      };

      class Foo: public Bar  // wrong
      {
         ...
      };

   2.6. Do not place spaces before colon in case labels.

      switch ( x )
      {
      case 0:  // right
         ...
      case 1 : // wrong
         ...
      }

   2.7. Place one space between a control statement and its left parenthesis.

      if ( x == 0 )  // right
      if( x == 0 )   // wrong
      if  ( x == 0 ) // wrong, unless spaces are used to vertically align a set of related control statements.

   2.8. Place one space to separate a control expression from its enclosing parentheses.

      if ( x == 0 )             // right
      if (x == 0)               // wrong
      if (  x == 0 )            // wrong, unless spaces are used to vertically align a set of related control statements.
      for ( i = 0; i < x; ++i ) // right
      for (i = 0; i < x; ++i )  // wrong
      while ( ++n  );           // wrong

   2.9. Do not place spaces between a function and its left parenthesis.

      SomeFunc( x );  // right
      SomeFunc ( x ); // wrong, unless spaces are used to vertically align arguments or parameters in a set of related items.

   2.10. Place one space to separate a list of function arguments or formal parameters from the opening and closing parentheses in a function call, function declaration, or function definition.

      SomeFunc( x, y, z ); // right
      SomeFunc( 0 );       // right
      SomeFunc(0);         // wrong
      SomeFunc(0 );        // wrong
      SomeFunc(  0 );      // wrong, unless spaces are used to vertically align arguments or parameters in a set of related items.

   2.11. Place one space after a comma separator in a list of arguments or formal parameters in a function call, function declaration, or function definition.

      SomeFunc( x, y, z );        // right
      SomeFunc( x,y,z );          // wrong
      SomeFunc( x+2, y*z, k+1 );  // right
      SomeFunc( x+2,y*z,k+1 );    // wrong
      SomeFunc( x+2,  y*z, k+1 ); // wrong, unless spaces are used to vertically align arguments or parameters in a set of related items.

   2.12. Do not place spaces to separate expressions from parentheses used to modify precedence of evaluation.

      x = (3 + y)*z;   // right
      x = ( 3 + y )*z; // wrong

   2.13. Do not place spaces to separate array subindex expressions from square brackets.

      x = r[i];   // right
      x = r[ i ]; // wrong

   2.14. Place one space to separate an allocation size expression from square brackets in a call to operator new[]. Do the same for array declarations.

      new Foo[ n ];         // right
      new Foo[n];           // wrong
      const char bar[ 10 ]; // right
      const char bar[10];   // wrong

   2.15. Place an empty line after a class member access specifier. Place an empty line before a class member access specifier, except the first one in a class.

      class Foo
      {
      public:  // ok: first access specifier followed by an empty line.

         Foo();
         ...
         int TheLastPublicMember();

      private: // ok: subsequent access specifiers preceded and followed by an empty line.

         Bar();
         ...
      };


### 3. Line Breaking


   3.1. In general, each statement should get its own line.

      if ( x )        // right
         y = 0;       // right
      if ( y ) z = 1; // generally wrong, but can be valid for very brief statements.

   3.2. An else statement must get its own line, and must be lined up with its corresponding if statement.

      if ( condition )
         DoSomething();
      else                  // right
         DoSomethingElse();

      if ( condition ) DoSomething(); else DoSomethingElse(); // wrong

      if ( condition ) DoSomething(); // wrong
      else DoSomethingElse();         // wrong

   3.3. An else if statement must lie on a single line as two words separated with one space, lined up with its corresponding if statement.

      if ( conditionOne )
         DoSomething();
      else if ( conditionTwo ) // right
         DoSomethingElse();

      if ( conditionOne )
         DoSomething();
      else
         if ( conditionTwo )   // wrong
            DoSomethingElse();

   3.4. Do not use else if after an if statement concluding with return.

      if ( haveFinished )
         return;
      if ( condition )      // right
         DoSomething();

      if ( haveFinished )
         return;
      else if ( condition ) // wrong
         DoSomething();


### 4. Braces


   4.1. Place each brace on its own line.

      class Bar
      {            // right
         ...
      };

      void Foo()
      {            // right
         ...
      }

      void Foo() { // wrong
         ...
      }

      if ( condition )
      {               // right
         ...
      }

      if ( condition ) { // wrong
         ...
      }

      if ( condition ) { ... }    // generally wrong, but can be admissible for very small code blocks.
      while ( condition ) { ... } // ...

      if ( condition ) {       // wrong
         DoSomething();
         DoSomethingElse(); }  // wrong

      static Mutex mutex;
      {                          // right - a critical section protected for parallel execution
         volatile AutoLock lock( mutex );
         /* Protected code */
         ...
      }

   4.2. Control clauses without a body must use empty braces after the control statement, separated with one space.

      for ( int i = 0; i < n; ++i ) {}  // right

      for ( int i = 0; i < n; ++i );    // wrong

      for ( int i = 0; i < n; ++i ) { } // wrong - should not place spaces between braces here

   4.3. One-line control clauses should not use braces unless comments are included before or after the one-line statement.

      if ( condition )
         DoSomething();    // right

      if ( condition )
      {                    // wrong
         DoSomething();
      }

      do
         DoSomething();    // right
      while ( condition );

      do
      {                    // wrong
         DoSomething();
      }
      while ( condition );

      if ( condition )
      {                    // right
         // This comment requires braces
         DoSomething();
      }

      if ( condition )
         // This comment requires braces
         DoSomething();    // wrong

      if ( condition )
      {
         DoSomething();
         DoSomethingElse();
      }
      else
         CallItFooBar();   // right

      if ( condition )
      {
         DoSomething();
         DoSomethingElse();
      }
      else
      {                    // can be admissible for readability in this case
         CallItFooBar();
      }

### 5. Names

   5.1. Use CamelCase. Capitalize all letters in acronyms. Capitalize the first letter (Pascal case) in class, struct, union, function and function member names. Lower-case (camel-back case) the first letter in variable and data member names.

      void Foo();              // right (function name, single word)
      void foo();              // wrong
      float SomeFunction();    // right (function name, several words)
      float some_function();   // wrong
      float Some_Function();   // wrong, unless there is a really good reason to use an underscore as a word separator
      class TheBarClass;       // right (class name)
      int Rect::Diagonal();    // right (function member)
      int Point::x;            // right (data member)
      String ConvertToHTML();  // right (function name, acronym)
      String ConvertToHtml();  // wrong

   5.2. Non-public, non-static data members must be prefixed with "m&#95;". Non-public, static data members must be prefixed with "s&#95;".

      class Foo
      {
      public:

         int xRatio;   // right, because this is a public data member.
         int m_result; // optional prefix, but prefer no prefix for global members.
         ...

      protected:

         int m_finalValue; // right (non-public, non-static data member)
         int m_FinalValue; // wrong
         int finalValue;   // wrong

         static float s_globalValue; // right (non-public, static data member)
         ...
      };

   5.3. In process instance implementation classes, data members implementing process parameters must be prefixed with "p&#95;". Data members implementing read-only output properties must be prefixed with "o&#95;".

      class MyProcessInstance : public ProcessImplementation
      {
         ...
      private:

         uint32 p_firstProcessParameter;  // right, data members implementing process parameters.
         String p_secondProcessParameter;

         double o_outputPropertyOne;      // right, data member implementing read-only output properties.
         uint16 o_outputPropertyTwo;

         int    m_dataMember; // right only if this is not an implementation of a process parameter or property.
      };

   5.4. In process interface implementation classes, function members implementing event handlers must be prefixed with either "e&#95;" or two underscores ("&#95;&#95;").

      class MyProcessInterface : public ProcessInterface
      {
         ...
      private:

         void __ValueUpdated( NumericEdit& sender, double value ); // function member is a GUI event handler.
         void e_Click( Button& sender, bool checked );  // also valid, but don't mix both styles in the same module.
      };

   5.5. Use snake_case and lowercase letters for class typedef names. Unless you have a very good reason to proceed otherwise, use the postfix "&#95;type" for class typedef names.

      class Foo : public Bar
      {
      public:

         typedef uint64        stream_length_type; // right
         typedef Bar:data_type data_type;          // right
         typedef int           IndexType;          // wrong
         typedef int64         stream_distance;    // warning - might lead to confusion without the _type postfix

         ...
      };

   5.6. In general, prefix Boolean variable and data member names with verbs such as "is", "has", "have", "did", etc.

      bool isValid;     // right
      bool hasFinished; // right
      bool sawHeaders;  // right
      bool valid;       // can be fine, but not as readable as "isValid".

   5.7. Prefix setter member functions with "Set".

      class Foo
      {
      public:

         void SetX( int x ) // right - valid setter function name
         {
            m_x = x;
         }

      private:

         int m_x;
      };

   5.8. When appropriate, prefix Boolean setter member functions with "Enable" and "Disable".

      class Foo
      {
      public:

         void SetTransparent( bool transparent = true ) // right - valid setter function name
         {
            m_transparent = transparent;
            ...
         }

         void EnableTransparency( bool enable = true )  // also right, and probably more readable
         {
            m_transparent = enable;
            ...
         }

         void DisableTransparency( bool disable = true )  // enable/disable idiom, nicely expressive
         {
            EnableTransparency( !disable );
         }

      private:

         bool m_transparent;
      };

   5.9. Do not prefix non-void getter member functions with "Get". Prefix void getter member functions with "Get".

      class Foo
      {
      public:

         void GetX( int& x ) const // right
         {
            x = m_x;
         }

         int X() const             // right
         {
            return m_x;
         }

         int GetX() const          // wrong
         {
            return m_x;
         }

      private:

         int m_x;
      };

   5.10. Do not use meaningless or redundant variable names in function declarations.

      void FileSize( size_type );              // right
      void FileSize( size_type size );         // wrong
      int EnableLogFile( bool = true );        // right
      int EnableLogFile( bool enable = true ); // wrong

   5.11. Use a block comment to signify the purpose of a Boolean function argument when its meaning is not obvious. Typically you should use formal parameter names in these comments.

      MaximizeWindow( window, true/*notify*/ ); // right

      int n = CountDocumentFiles( targetDir, false/*includePlainTextFiles*/ ); // right
      int n = CountDocumentFiles( targetDir, false );                          // wrong - false what?

      EnableMaximization( false ); // right - obvious argument meaning

   5.12. #define and #ifdef header guards should be named exactly the same as the source file (including case), replacing the dot suffix separator with an underscore, and prefixed with two underscores. Use line comments to mark the corresponding #endif closing directives.

      #ifndef __MyProcessInstance_h // right for MyProcessInstance.h
      #define __MyProcessInstance_h

      namespace pcl
      {
      ...
      } // pcl

      #endif // __MyProcessInstance_h


### 6. Pointers and References

   6.1. Do not place spaces between indirection and reference operators and type names.

      int* pointerToObject;  // right
      int *pointerToObject;  // wrong
      int * pointerToObject; // wrong

      void CenterPoint( double& x, double& y, double* values ); // right
      void CenterPoint( double &x, double &y, double *values ); // wrong

   6.2. Output reference and pointer parameters should be placed at the beginning of the list of function parameters, always *before* input parameters.

      void GetPositionVector( double& x, double& y, double& z,      // right
                              int planetNumber, double julianDay );

      void GetPositionVector( int planetNumber, double julianDay,   // wrong
                              double& x, double& y, double& z );



### 7. Virtual Member Functions

   7.1. Never omit the word 'virtual' in a reimplementation of a virtual or pure virtual member function.

      class Base
      {
         virtual void OneMember();
         virtual int OtherMember();
      };

      class Derived : public Base
      {
         virtual void OneMember(); // right
         int OtherMember();        // wrong
      };


*******************************************************************************
