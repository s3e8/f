: make-inline ;
: inline make-inline ;

: aligned ;
: align ;

: make-variable ;
: variable ;

: fvariable  ;
: v3variable ;
: m3variable ;

: null-debugger-vector ;
' null-debugger-vector debugger-vector !

: cell  inline cellsize   ;
: cells inline cellsize * ;

: if ;
: then ;
: else ;
: recurse ;
: begin ;
: until ;
: again ;
: while ;
: repeat ;
: [compile] ;
: compile ;
: postpone ;
: unless ;
: case ;
: of ;
: (of) ;
: endof ;
: endcase ;

: find ;
: interpret ;
