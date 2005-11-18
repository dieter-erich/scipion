/***************************************************************************
 *
 * Authors:     Carlos Oscar S. Sorzano (coss@cnb.uam.es)
 *
 * Unidad de  Bioinformatica of Centro Nacional de Biotecnologia , CSIC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or   
 * (at your option) any later version.                                 
 *                                                                     
 * This program is distributed in the hope that it will be useful,     
 * but WITHOUT ANY WARRANTY; without even the implied warranty of      
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       
 * GNU General Public License for more details.                        
 *                                                                     
 * You should have received a copy of the GNU General Public License   
 * along with this program; if not, write to the Free Software         
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA            
 * 02111-1307  USA                                                     
 *                                                                     
 *  All comments concerning this program package may be sent to the    
 *  e-mail address 'xmipp@cnb.uam.es'                                  
 ***************************************************************************/
/* ------------------------------------------------------------------------- */
/* VECTORS                                                                   */
/* ------------------------------------------------------------------------- */
#ifndef _XMIPPVECTOR_HH
#   define _XMIPPVECTOR_HH

/* ************************************************************************* */
/* INCLUDES                                                                  */
/* ************************************************************************* */
#include "xmippFuncs.hh"
#include <fstream>
#include <math.h>
#include <complex>

#define maT  matrix1D<T>
#define maT1 matrix1D<T1>
#define maTC matrix1D< complex<double> >

/* ************************************************************************* */
/* FORWARD DEFINITIONS                                                       */
/* ************************************************************************* */
#include "Src/MultidimFriends.inc"
string FtoA(float F, int _width, int _prec);

/* ************************************************************************* */
/* CLASS DEFINITION AND PROTOTYPES                                           */
/* ************************************************************************* */
/**@name Xmipp Vectors*/
//@{
/* Speed up ------------------------------------------------------------- */
/**@name Speed up macros
   This macros are defined to allow high speed in critical parts of
   your program. They shouldn't be used systematically as usually there
   is no checking on the correctness of the operation you are performing.
   Speed comes from three facts: first, they are macros and no function
   call is performed (although most of the critical functions are
   inline functions), there is no checking on the correctness of the
   operation (it could be wrong and you are not warned of it), and
   destination vectors are not returned saving time in the copy
   constructor and in the creation/destruction of temporary vectors.*/
//@{

/**@name Size and shape*/
//@{
/** TRUE if both arrays have the same shape.
    Two arrays have the same shape if they have the same size and the
    same starting point. Be aware that this is a macro which simplifies to
    a boolean. */
#define SAME_SHAPE1D(v1,v2) \
    (XSIZE(v1)==XSIZE(v2) && \
     STARTINGX(v1)==STARTINGX(v2))

/** Returns the first valid logical index.
    \\Ex: int org=STARTINGX(v);*/
#define STARTINGX(v)  ((v).xinit)

/** Returns the last valid logical index.
    \\Ex: int fin=FINISHINGX(v);*/
#define FINISHINGX(v) ((v).xinit+(v).xdim-1)

/** Access to X dimension (size).
    This is a macro equivalent to \Ref{get_dim()} or \Ref{ColNo()}
    \\ Ex:
    \begin{verbatim}
    // Set to 0 1 element out of 2
    for (int i=0; i<XSIZE(v); i +=2) DIRECT_VEC_ELEM(v,i)=0;
    \end{verbatim}*/
#define XSIZE(v) ((v).xdim)

/** For all elements in the array.
    This macro is used to generate loops for the vector in an easy manner.
    It defines an internal index 'i' which ranges the vector using its
    mathematical definition (ie, logical access).
    \\Ex:
    \begin{verbatim}
    FOR_ALL_ELEMENTS_IN_MATRIX1D(v) {
       cout << v(i) << " ";
    }
    \end{verbatim}*/
#define FOR_ALL_ELEMENTS_IN_MATRIX1D(v) \
    for (int i=STARTINGX(v); i<=FINISHINGX(v); i++)

/** For all elements in the array between corners.
    This macro is used to generate loops for a vector in an easy manner.
    It needs an externally defined matrix1D<double> r(1).
    Then XX(r) ranges from (int) XX(corner1) to
    (int) XX(corner2) (included limits) (notice that corner1 and corner2 need
    only to be matrix1D). 
    \\Ex:
    \begin{verbatim}
    matrix1D<double> corner1(1), corner2(1), r(1);
    XX(corner1)=-1; XX(corner2)=1;
    FOR_ALL_ELEMENTS_IN_MATRIX1D_BETWEEN(corner1,corner2) {
       cout << v(XX(r)) << " ";
    }
    \end{verbatim}*/
#define FOR_ALL_ELEMENTS_IN_MATRIX1D_BETWEEN(corner1,corner2) \
    for (XX(r)=(int) XX((corner1)); XX(r)<=(int) XX((corner2)); XX(r)++)

/** For all elements in common.
    This macro is used to generate loops for all the elements logically
    in common between two vectors in an easy manner.
    Then i (locally defined) ranges from
    MAX(STARTINGX(V1),STARTINGX(V2)) to MIN(FINISHINGX(V1),FINISHINGX(V2))
    (included limits) respectively. You need to define SPEED_UP_temps.
    \\Ex:
    \begin{verbatim}
    matrix2D<double> v1(10), v2(20);
    v1.set_Xmipp_origin();
    v2.set_Xmipp_origin();
    FOR_ALL_ELEMENTS_IN_COMMON_IN_MATRIX1D(v1,v2) {
       ...
    }
    \end{verbatim}*/
#define FOR_ALL_ELEMENTS_IN_COMMON_IN_MATRIX1D(v1,v2) \
    ispduptmp4=MAX(STARTINGX(v1), STARTINGX(v2)); \
    ispduptmp5=MIN(FINISHINGX(v1),FINISHINGX(v2)); \
    for (int i=ispduptmp4; i<=ispduptmp5; i++)

/** For all elements in the array, accessed physically.
    This macro is used to generate loops for the vector in an easy way
    using physical indexes.
    It defines internal the index 'i' which ranges the vector using
    its physical definition.
    \\Ex:
    \begin{verbatim}
    FOR_ALL_DIRECT_ELEMENTS_IN_MATRIX1D(v) {
       cout << DIRECT_MAT_ELEM(v,i) << " ";
    }
    \end{verbatim} */
#define FOR_ALL_DIRECT_ELEMENTS_IN_MATRIX1D(v) \
    for (int i=0; i<XSIZE(v); i++) 

//@}

/**@name Memory access */
//@{
/** Vector element: Logical access.
    \\ Ex: VEC_ELEM(v,-2)=1;
    \\ Ex: val=VEC_ELEM(v,-2);*/
#define VEC_ELEM(v,i) MULTIDIM_ELEM(v,(i)-((v).xinit))

/** Vector element: Physical access.
    Be careful because this is physical access, usually vectors follow
    the C convention of starting index==0. This function should not be
    used as it goes against the vector library philosophy unless you
    explicitly want to access directly to any value in the vector
    without taking into account its logical position
    \\ Ex: DIRECT_VEC_ELEM(v,0)=1;
    \\ Ex: val=DIRECT_VEC_ELEM(v,0);*/
#define DIRECT_VEC_ELEM(v,i) MULTIDIM_ELEM(v,i)

/** A short alias to previous function. */
#define dVi(v,i) DIRECT_VEC_ELEM(v,i)

/** Access to X component.
    \\ Ex: XX(v)=1;
    \\ Ex: val=XX(v); */
#define XX(v) MULTIDIM_ELEM(v,0)

/** Access to Y component.
    \\ Ex: YY(v)=1;
    \\ Ex: val=YY(v); */
#define YY(v) MULTIDIM_ELEM(v,1)

/** Access to Z component.
    \\ Ex: ZZ(v)=1;
    \\ Ex: val=ZZ(v); */
#define ZZ(v) MULTIDIM_ELEM(v,2)

/** Array access.
    This macro gives you access to the array (T *).
    \\ Ex: cout << "This is an int *" << VEC_ARRAY(v) << endl; */
#define VEC_ARRAY(v) MULTIDIM_ARRAY(v)
//@}

/**@name Geometry vectors
   The vectors involved in these macros should be created with the
   correct size before entering in them. These macros allow a fast
   creation of R2 and R3 vectors. */
//@{
/** Creates vector in R2.
    The vector must be created beforehand to the correct size. After
    this macro the vector is (x,y) in R2.
    \\Ex: matrix1D<double> v(2); VECTOR_R2(v,1,2); */
#define VECTOR_R2(v,x,y) {\
    XX(v)=x; YY(v)=y;}

/** Creates vector in R3.
    The vector must be created beforehand to the correct size. After
    this macro the vector is (x,y,z) in R3.
    \\Ex: matrix1D<double> v(3); VECTOR_R2(v,1,2,1); */
#define VECTOR_R3(v,x,y,z) {\
   XX(v)=x; YY(v)=y; ZZ(v)=z;}
//@}

/**@name Arithmethic operations
   The vectors involved in these macros should be created with the
   correct size before entering in them. These macros allow a fast
   operation on R2 and R3 vectors. */
//@{
/**@name R2 operations */
//@{
/** Adding two R2 vectors (a=b+c).
   \\Ex: matrix1D<double> a(2), b(2), c(2); ...; V2_PLUS_V2(a,b,c); */
#define V2_PLUS_V2(a,b,c) {\
   XX(a)=XX(b)+XX(c); \
   YY(a)=YY(b)+YY(c);}

/** Substracting two R2 vectors (a=b-c).
   \\Ex: matrix1D<double> a(2), b(2), c(2); ...; V2_MINUS_V2(a,b,c); */
#define V2_MINUS_V2(a,b,c) {\
   XX(a)=XX(b)-XX(c); \
   YY(a)=YY(b)-YY(c);}

/** Adding/substracting a constant to a R2 vector (a=b-k).
   \\Ex: matrix1D<double> a(2), b(2); double k; ...; V2_PLUS_CT(a,b,k);
   \\Ex: matrix1D<double> a(2), b(2); double k; ...; V2_PLUS_CT(a,b,-k); */
#define V2_PLUS_CT(a,b,k) {\
   XX(a)=XX(b)+(k); \
   YY(a)=YY(b)+(k);}

/** Multiplying/dividing by a constant a R2 vector (a=b*k).
   \\Ex: matrix1D<double> a(2), b(2); double k; ...; V2_BY_CT(a,b,k);
   \\Ex: matrix1D<double> a(2), b(2); double k; ...; V2_BY_CT(a,b,1/k); */
#define V2_BY_CT(a,b,k) {\
   XX(a)=XX(b)*(k); \
   YY(a)=YY(b)*(k);}
//@}

/**@name R3 operations */
//@{
/** Adding two R3 vectors (a=b+c).
   \\Ex: matrix1D<double> a(3), b(3), c(3); ...; V3_PLUS_V3(a,b,c); */
#define V3_PLUS_V3(a,b,c) {\
   XX(a)=XX(b)+XX(c); \
   YY(a)=YY(b)+YY(c); \
   ZZ(a)=ZZ(b)+ZZ(c);}

/** Substracting two R3 vectors (a=b-c).
   \\Ex: matrix1D<double> a(3), b(3), c(3); ...; V3_MINUS_V3(a,b,c); */
#define V3_MINUS_V3(a,b,c) {\
   XX(a)=XX(b)-XX(c); \
   YY(a)=YY(b)-YY(c); \
   ZZ(a)=ZZ(b)-ZZ(c);}

/** Adding/substracting a constant to a R3 vector (a=b-k).
   \\Ex: matrix1D<double> a(3), b(3); double k; ...; V3_PLUS_CT(a,b,k);
   \\Ex: matrix1D<double> a(3), b(3); double k; ...; V3_PLUS_CT(a,b,-k); */
#define V3_PLUS_CT(a,b,c) {\
   XX(a)=XX(b)+(c); \
   YY(a)=YY(b)+(c); \
   ZZ(a)=ZZ(b)+(c);}

/** Multiplying/dividing by a constant a R3 vector (a=b*k).
   \\Ex: matrix1D<double> a(3), b(3); double k; ...; V3_BY_CT(a,b,k);
   \\Ex: matrix1D<double> a(3), b(3); double k; ...; V3_BY_CT(a,b,1/k); */
#define V3_BY_CT(a,b,c) {\
   XX(a)=XX(b)*(c); \
   YY(a)=YY(b)*(c); \
   ZZ(a)=ZZ(b)*(c);}
//@}
//@}
//@}

//@Include: MultidimCommon.hh
#include "MultidimCommon.hh"
/// Template class for Xmipp vectors
template <class T> class matrix1D {
#include "Src/MultidimBasic.hh"
/**@name Common functions to all multidimensional arrays
   A set of methods are always the same for any multidimensional array.
   Have a look on the more detailed structure. */
public:
//@{
//@Include: Src/MultidimBasic.hh
//@}

/* Structure =============================================================== */
// Although the structure is defined as public it should not be used by
// the library user, there are functions enough to handle everything. This is
// done so to speed up the library
public:
   int        xdim;              // dimension of array [0...xdim-1]
   int        xinit;             // indexes of array [xinit...xinit+xdim-1]
   bool       row;               // 0=column vector (default), 1=row vector

/* Procedures ============================================================== */
public:
   /* Constructors/Destructor ---------------------------------------------- */
   /**@name Constructors*/
   //@{
   /** Empty constructor.
       The empty constructor creates a vector with no memory associated,
       origin=0, size=0, no statistics, ... You can choose between a column
       vector (by default), or a row one.
       \\Ex: matrix1D<double> v1;
       \\Ex: matrix1D<double> v1('y');
       \\--> both are examples of empty column vectors
       \\Ex: matrix1D<int> v1('n');
       \\--> empty row vector */
   matrix1D(const char column='y')
      {core_init(); init_shape(column); __spcdim=1;}

   /** Dimension constructor.
       The dimension constructor creates a vector with memory associated
       (but not assigned to anything, could be full of garbage)
       origin=0, size=the given one
       You can choose between a column vector (by default), or a row one.
       \\Ex: matrix1D<double> v1(6);
       \\Ex: matrix1D<double> v1(6,'y');
       \\--> both are examples of column vectors of dimensions 6
       \\Ex: matrix1D<int> v1('n');
       \\--> empty row vector */
   matrix1D(int dim, const char column='y')
      {core_init(); init_shape(column); resize(dim); __spcdim=1;}

   /** Copy constructor.
       The created vector is a perfect copy of the input vector but
       with a different memory assignment.
       \\Ex: matrix1D<double> v2(v1); */
   matrix1D(const vT &v) {core_init(); init_shape(); *this=v;}
   
   // Destructor
   ~matrix1D() {core_deallocate();}
   //@}

   /* Initialisation ------------------------------------------------------- */
   /**@name Initialisation*/
   //@{
   /** Linear initialisation.
       The vector is filled with values increasing/decreasing linearly
       within a range or at given steps.
       
       Increment functionality: The default increment is 1, the initial
       point is incremented by this value until the upper limit is reached.
       This is the default working mode for the function.
       \\Ex: v1.init_linear(1,3); --> v1=[1 2 3]
       \\Ex: v1.init_linear(1.5,3.1); --> v1=[1.5 2.5]
       \\Ex: v1.init_linear(0,10,3); --> v1=[0 3 6 9]
       \\Ex: v1.init_linear(0,10,3,"incr"); --> v1=[0 3 6 9]
       
       Step functionality: The given range is divided in as many points as
       indicated (in the example 6 points).
       \\Ex: v1.init_linear(0,10,6,"steps"); --> v1=[0 2 4 6 8 10] */
   void init_linear(T minF, T maxF, int n=1,
         const string &mode="incr") {
         double slope;
         int steps;

         if (mode=="incr") {
            steps=1+(int) FLOOR((double)ABS((maxF-minF))/((double) n));
            slope=n*SGN(maxF-minF);
         } else if (mode=="steps") {
            steps=n;
            slope=(maxF-minF)/(steps-1);
         } else 
            REPORT_ERROR(1005,"Init_linear: Mode not supported ("+mode+")");

         if (steps==0) clear();
         else {   
            resize(steps);
            for (int i=0; i<steps; i++)
               VEC_ELEM(*this,i)=(T) ((double)minF+slope*i);
         }
      }

   /** Zero initialisation with a new dimension.
       \\Ex: v1.init_zeros(6);*/         
   void init_zeros(int dim) {resize(dim); init_constant((T)0);}
   //@}

   /* Memory related ------------------------------------------------------- */
   /**@name Size and shape
      The shape of a vector is defined by its origin, its size and if
      it is a column or a row vector. The size is clear, and the origin
      is the logical position of the first real position of the array. For
      instance, if we have a vector of dimension 5 and origin -2, this means
      that the array is representing the logical positions [-2 -1 0 1 2], we
      could access to any of these positions (Ex: v(-2)=3;) and actually
      any try to access to a position related to 5 (Ex: v(4)=3;), although
      it physically exists, is not logically correct and hence it will
      throw an exception. The starting and finishing positions for this
      sample vector are -2 and 2 respectively, and the "for" iterations
      through the vector should include these 2 values if you want to
      cover the whole vector
      
      \begin{verbatim}
         for (int i=STARTINGX(v); i<=FINISHINGX(v); i++)
            VEC_ELEM(v,i) += 1;
      \end{verbatim} */
      
   //@{
   /** Init shape.
       xdim=0, startingx=0. The column or row mode is set according to
       input argument */
   void init_shape(const char column='y')
      {xinit=0; xdim=0;
       if      (column=='y') row=false;
       else if (column=='n') row=true;
       else    REPORT_ERROR(1004,"Not recognized type for vector creation");}

   /** Copy shape.
       Copy shape variables from a pattern AND THE ARRAY IS RESIZED */
   template <class T1>
      void copy_shape(const maT1 &v)
         {if (XSIZE(*this)!=XSIZE(v)) resize(XSIZE(v));
          STARTINGX(*this)=STARTINGX(v);
          row=v.row;}

   /** Resize to a given size.
       This function resize the actual array to the given size. The origin
       is not modified. If the actual array is larger than the pattern
       then the trailing values are lost, if it is smaller then 0's are
       added at the end. An exception is thrown if there is no memory.
       \\Ex: v1.resize(3);
   */
   void resize(int Xdim) {
      if (Xdim==XSIZE(*this)) return;
      if (Xdim<=0)           {clear(); return;}

      // Ask for memory   
      T* new_m=new T [Xdim];
      if (new_m==NULL) REPORT_ERROR(1001,"Resize: no memory left");

      // Copy needed elements, fill with 0 if necessary
      for (int i=0; i<Xdim; i++)
         if (i<XSIZE(*this)) new_m[i]=DIRECT_VEC_ELEM(*this,i);
         else                new_m[i]=0;

      // deallocate old vector
      core_deallocate();

      // assign *this vector to the newly created
      MULTIDIM_ARRAY(*this)=new_m;
      XSIZE(*this)=Xdim;
      __dim=Xdim;
   }

   /** Produce a vector suitable for working with Numerical Recipes.
       This function must be used only as
       a preparation for routines which need that the first physical
       index is 1 and not 0 as it usually is in C. In fact the vector
       provided for Numerical recipes is exactly this same one but
       with the indexes changed.
       Click here to see an \URL[example]{../Extra_Docs/examples.html#LU} */
   T * adapt_for_numerical_recipes() const {return __m-1;}

   /** Kill an array produced for numerical recipes.
       Nothing needs to be done in fact. */
   void kill_adaptation_for_numerical_recipes(T *m) const {}

   /** Intersects.
       TRUE if this array intersects with the segment defined by the
       arguments (x0 is the starting X).*/
   bool intersects(double x0, double xdim) const;

   /** Outside.
       TRUE if the logical index given is outside the definition region
       of this array. */
   bool outside(int i) const;

   /** isBorder.
       TRUE if the logical index given belong to the border of the matrix */
   bool isBorder(int i);

   /** Set logical origin in Xmipp fashion.
       This function adjust the starting points in the vector such that
       the center of the vector is defined in the Xmipp fashion.
       See \Ref{FIRST_XMIPP_INDEX}
       \\Ex: v1.set_Xmipp_origin();
       @see Conventions */
   void set_Xmipp_origin() {xinit=FIRST_XMIPP_INDEX(xdim);}

   /** Move origin to.
       This function adjust logical indexes such that the Xmipp origin
       of the array moves to the specified position. For instance, an array
       whose x indexes go from -1 to 1, if we move the origin to 4, then
       the x indexes go from 3 to 5. This is very useful for convolution
       operations where you only need to move the logical starting of the
       array. 
       See \Ref{FIRST_XMIPP_INDEX} */
   void move_origin_to(int i)
      {STARTINGX(*this)=i+FIRST_XMIPP_INDEX(XSIZE(*this));}

   /** Sets the origin.
       The logical position of the first physical position is set with
       this function. By default the origin is 0 that is the standard
       convention in C.
       \\Ex: v.startingX()=-2; */
   int& startingX() {return xinit;}
   
   /** Returns the first valid logical index.
       \\Ex: int org=v.startingX();*/
   int startingX() const {return xinit;}
   
   /** Returns the last valid logical index.
       \\Ex: int fin=v.finishingX();*/
   int finishingX() const {return xinit+xdim-1;}
   
   /** Returns the vector dimension.
       \\ Ex: int dim=v.get_dim();*/
   int  get_dim() const {return xdim;}
   
   /** Returns the vector dimension.
       \\ Ex: int dim; v.get_dim(dim);*/
   void get_dim(int &d) const {d=xdim;}
   
   /** True if vector is a row.
       \\ Ex: if (v.isRow()) cout << "v is a row vector\n";*/
   int  isRow()  const  {return row;}
   
   /** True if vector is a column.
       \\ Ex: if (v.isCol()) cout << "v is a column vector\n";*/
   int  isCol()  const  {return !row;}
   
   /** Forces the vector to be a row vector.
       \\Ex: v.setRow();*/
   void setRow() {row=1;}

   /** Forces the vector to be a column vector.
       \\Ex: v.setCol();*/
   void setCol() {row=0;}

   /** Same shape.
       Returns true if this object has got the same shape (origin and
       size) than the argument*/
   bool same_shape(const maT &op) const
      {return SAME_SHAPE1D(*this, op);}

   //@}

   /* Information Extraction/Setting --------------------------------------- */
   /**@name Memory access
      This functions allows you to access to the vector elements.*/
   //@{
   /** Vector element access.
       Returns the value of a vector logical position. In our example we could
       access from v(-2) to v(2). The elements can be used either by value
       or by reference. An exception is thrown if the index is outside the
       logical range.
       \\ Ex: v(-2)=1;
       \\ Ex: val=v(-2);*/
   T&   operator () (int i) const {
        if (i>=xinit+xdim || i<xinit)
           REPORT_ERROR(1003,"Vector subscript not defined for this vector");
        return MULTIDIM_ELEM(*this,i-STARTINGX(*this));}
   
   /** Matrix element access via double vector.
       Returns the value of a matrix logical position, but this time the
       element position is determined by a R1 vector.
       The elements can be used either by value or by reference.
       No check is done on the validity of the vector. */
   T&   operator () (const matrix1D<double> &v) const
        {return VEC_ELEM(*this,ROUND(XX(v)));}

   /** Matrix element access via integer vector.*/
   T&   operator () (const matrix1D<int> &v) const
        {return VEC_ELEM(*this,XX(v));}

   /** Access to X component.
       A special case of vectors are those of 2 or 3 components, they are
       treated as vectors in R2 or R3 for which the X component is defined.
       This function allows you to access in a more elegant way to this
       component than the previous function, but you could always access
       to the first position of the array. An exception is thrown if the
       vector doesn't belong to R2 or R3.
       \\ Ex: v.X()=1;
       \\ Ex: val=v.X();*/
   T&   X() const {
        if (xdim>3 || xdim<=0)
           REPORT_ERROR(1003,"X: Subscript not defined for this dimension");
        return MULTIDIM_ELEM(*this,0);}

   /** Access to Y component.
       A special case of vectors are those of 2 or 3 components, they are
       treated as vectors in R2 or R3 for which the Y component is defined.
       This function allows you to access in a more elegant way to this
       component than the previous function, but you could always access
       to the second position of the array. An exception is thrown if the
       vector doesn't belong to R2 or R3.
       \\ Ex: v.Y()=1;
       \\ Ex: val=v.Y();*/
   T&   Y() const {
        if (xdim>3 || xdim<=1)
           REPORT_ERROR(1003,"Y: Subscript not defined for this dimension");
        return MULTIDIM_ELEM(*this,1);}

   /** Access to Z component.
       A special case of vectors are those of 3 components, they are
       treated as vectors in R3 for which the Z component is defined.
       This function allows you to access in a more elegant way to this
       component than the previous function, but you could always access
       to the second position of the array. An exception is thrown if the
       vector doesn't belong to R3.
       \\ Ex: v.Z()=1;
       \\ Ex: val=v.Z();*/
   T&   Z() const {
        if (xdim!=3)
           REPORT_ERROR(1003,"Z: Subscript not defined for this dimension");
        return MULTIDIM_ELEM(*this,2);}

   /** Logical to physical index translation.
       This function returns the physical position of a logical one. See
       \URL[Conventions]{../../../Extra_Docs/Conventions.html}
       for more information about these two different accesses.
       \\ Ex: v.logical2physical(i_log,i_phys); */
   void logical2physical(int i_log, int &i_phys) const
       {i_phys=i_log-STARTINGX(*this);}
   
   /** Physical to logical index translation.
       This function returns the logical position of a physical one. See
       \URL[Conventions]{../../../Extra_Docs/Conventions.html}
       for more information about these two different accesses.
       \\ Ex: v.physical2logical(i_phys,i_log); */
   void physical2logical(int i_phys, int &i_log) const
       {i_log=i_phys+STARTINGX(*this);}
   //@}

   /* Other utilities ------------------------------------------------------ */
   /**@name Utilities*/
   //@{
   /** This function must take two arrays of the same size, and operate
      element by element according to the operation required. This is the
      function which really implements the operations. Simple calls to it
      perform much faster than calls to the corresponding operators.
      Although it is supposed to be a hidden function not useable by
      normal programmers.
      
      It must be implemented in every Matrix module, this is so because
      of the Matrix2D, for which the multiplication is not a component
      by component multiplication but an algebraic one.*/
   friend void array_by_array(const maT &op1, const maT &op2, maT &result,
      char operation) {
         if (!op1.same_shape(op2))
            REPORT_ERROR(1007,
               (string)"Array_by_array: different shapes ("+operation+")");
         if (operation=='x') operation='*';
         result.resize(op1);
         core_array_by_array(op1, op2, result, operation);
      }

   /** Vector by matrix.
       Algebraic vector by matrix multiplication. This function is actually
       implemented in xmippMatrices2D */
   vT operator *(const mT &M);

   /** Algebraic transpose of vector.
       You can use the transpose in as complex expressions as you like. The
       origin of the vector is not changed.
       \\Ex: v2=v1.transpose();*/
   vT transpose() const
      {vT temp(*this); temp.self_transpose(); return temp;}
   
   /** Algebraic transpose of vector.
       The same as before but the result is stored in this same object. */
   void self_transpose() {row=!row;}

   /** Reverse vector values.
       The first value is now the last one, the second value is now the one
       before the last one, ... the shape (column or vector) and the origin
       of the vector are not modified. For example, a vector of values
       [0 1 2 3] would give [3 2 1 0].
       \\Ex: v2=v1.reverse();*/
   vT reverse() const
      {vT temp(*this); temp.self_reverse(); return temp;}
   
   /** Reverse vector values, keep in this object */
   void self_reverse() {
      int imax=(int)(XSIZE(*this)-1)/2;
      for (int i=0; i<=imax; i++) {
         T aux;
         SWAP(MULTIDIM_ELEM(*this,i),MULTIDIM_ELEM(*this,XSIZE(*this)-1-i),aux);
      }
   }

   /** Module of the vector.
       This module is defined as the square root of the sum of the squared
       components. Euclidean norm of the vector.
       \\Ex: double mod=v.module();*/
   double module() const {return sqrt(sum2());}
   
   /** Angle of the vector.
       Supposing this vector is in R2 this function returns the angle of
       this vector with X axis, ie, atan2(YY(v),XX(v)) */
   double angle() {return atan2(YY(*this),XX(*this));}

   /** Normalize vector.
       Generate a vector of module 1. This is achieved dividing the
       vector its euclidean norm.
       \\Ex: v1=v1.normalize();*/
   vT normalize() const
      {vT temp(*this); temp.self_normalize(); return temp;}

   /** Normalize this vector, store the result here */
   void self_normalize()
      {double m=module();
      if (ABS(m)>XMIPP_EQUAL_ACCURACY) *this /= m; else init_zeros();}
   
   /** Sort vector elements.
       Sort in ascending order the vector elements. You can use the "reverse"
       function to sort in descending order.
       \\Ex: v2=v1.sort();*/
   vT sort() const {
      vT temp;
      matrix1D<double> aux;

      if (XSIZE(*this)==0) return temp;

      // Initialise data
      type_cast(*this, aux);

      // Sort
      double * aux_array=aux.adapt_for_numerical_recipes();
      qcksrt(xdim,aux_array);

      type_cast(aux,temp);
      return temp;
   }
   
   /** Gives a vector with the indexes for a sorted vector.
       This function returns the indexes of a sorted vector. The input vector
       is not modified at all. For instance, if the input vector is
       [3 2 -1 0] th result of this function would be [2 3 1 0] meaning
       that the lowest value is at index 2, then comes the element at
       index 3, ...
       \\ Ex: v2=v1.index_sort();*/
   matrix1D<int> index_sort() const {
      matrix1D<int>   indx;
      matrix1D<double> temp;

      if (XSIZE(*this)==0) return indx;
      if (XSIZE(*this)==1) {
         indx.resize(1); indx(0)=1;
         return indx;
      }

      // Initialise data
      indx.resize(xdim);
      type_cast(*this,temp);

      // Sort indexes
      double * temp_array=temp.adapt_for_numerical_recipes();
      int   * indx_array=indx.adapt_for_numerical_recipes();
      indexx(xdim,temp_array,indx_array);

      return indx;
   }

   /** Put a window to vector.
       The vector is windowed within the two indexes given to this function.
       Indexes always refer to logical indexes. If an index is outside the
       actual vector range then the vector is padded winit_value. In the following examples suppose that v1=
       [-2 -1 0 1 2] and that the origin is -2.
       \\Ex: v1.window(-1,2);
       \\--> v1=[-1 0 1 2]; v1.startingX()==-1
       \\Ex: v1.window(-3,1);
       \\--> v1=[0 -2 -1 0 1]; v1.startingX()==-3 */
   void window(int x0, int xF, T init_value=0) {
      vT result(xF-x0+1);
      STARTINGX(result)=x0;

      for (int j=x0; j<=xF; j++)
          if (j>=STARTINGX(*this) && j<=FINISHINGX(*this))
                  VEC_ELEM(result,j)=VEC_ELEM(*this,j);
          else
                  VEC_ELEM(result,j)=init_value;

      *this=result;
   }

   /** Maximum element.
       This function returns the index of the maximum element of an array.
       array(i). Returns -1 if the array is empty*/
   void max_index(int &imax) const {
      if (XSIZE(*this)==0) {imax=-1; return;}
      imax=0;
      T   max=VEC_ELEM(*this,imax);
      FOR_ALL_ELEMENTS_IN_MATRIX1D(*this)
         if (VEC_ELEM(*this,i)>max) {max=VEC_ELEM(*this,i); imax=i;}
   }

   /** Minimum element.
       This function returns the index of the minimum element of an array.
       array(i). Returns -1 if the array is empty */
   void min_index(int &imin) const {
      if (XSIZE(*this)==0) {imin=-1; return;}
      imin=0;
      T   min=VEC_ELEM(*this,imin);
      FOR_ALL_ELEMENTS_IN_MATRIX1D(*this)
         if (VEC_ELEM(*this,i)<min) {min=VEC_ELEM(*this,i); imin=i;}
   }
   
   /** Show using gnuplot.
       This function uses gnuplot to plot this vector. You must supply
       the xlabel, ylabel, and title. */
   void show_with_gnuplot(const string &xlabel, const string &title) {
      FileName fn_tmp;
      fn_tmp.init_random(10);
      write((string)"PPP"+fn_tmp+".txt");
      ofstream fh_gplot;
      fh_gplot.open(((string)"PPP"+fn_tmp+".gpl").c_str());
      if (!fh_gplot)
         REPORT_ERROR(1,(string)"vector::show_with_gnuplot: Cannot open PPP"+
            fn_tmp+".gpl for output");
      fh_gplot << "set xlabel \""+xlabel+"\"\n";
      fh_gplot << "plot \"PPP"+fn_tmp+".txt\" title \""+title+"\" w l\n";
      fh_gplot << "pause 300 \"\"\n";
      fh_gplot.close();
      system(((string)"(gnuplot PPP"+fn_tmp+".gpl; rm PPP"+fn_tmp+
         ".txt PPP"+fn_tmp+".gpl) &").c_str());
   }
   //@}
};

#include "Src/MultidimFriends_implementation.hh"

/**@name Related functions
   These functions are not methods of matrix1D */
//@{

/* Geometry ---------------------------------------------------------------- */
/**@name Geometry
   These functions allow you to create easily R2 and R3 vectors. */
//@{
/** Creates vector in R2.
    After this function the vector is (x,y) in R2.
    \\Ex: matrix1D<double> v=vector_R2(1,2); */
matrix1D<double> vector_R2(double x, double y);

/** Creates vector in R3.
    After this function the vector is (x,y,z) in R3.
    \\Ex: matrix1D<double> v=vector_R2(1,2,1); */
matrix1D<double> vector_R3(double x, double y, double z);

/** Creates an integer vector in Z3. */
matrix1D<int>    vector_R3(int x, int y, int z);

/** Dot product.
    Given any two vectors in Rn (n-dimensional vector), this function
    returns the dot product of both. If the vectors are not of the same
    size or shape then an exception is thrown. The dot product is
    defined as the sum of the component by component multiplication.
    For the R3 vectors (V1x,V1y,V1z), (V2x, V2y, V2z) the result is
    V1x*V2x + V1y*V2y + V1z*V2z.
    \\ Ex:
    \begin{verbatim}
    matrix1D<double> v1(1000);
    v1.init_random(0,10,"gaussian");
    cout << "The power of this vector should be 100 and is "
         << dot_product(v1,v1) << endl;
   \end{verbatim}*/
template <class T> T dot_product(const matrix1D<T> &v1,
   const matrix1D<T> &v2) {
   if (!v1.same_shape(v2))
      REPORT_ERROR(1002,"Dot product: vectors of different size or shape");
   T accumulate=0;
   FOR_ALL_ELEMENTS_IN_MULTIDIM_ARRAY(v1)
      accumulate += MULTIDIM_ELEM(v1,i)*MULTIDIM_ELEM(v2,i);
   return accumulate;
}

/** Vector product in R3.
    This function takes two R3 vectors and compute their vectorial product.
    For two vectors (V1x,V1y,V1z), (V2x, V2y, V2z) the result is
    (V1y*V2z-V1z*v2y, V1z*V2x-V1x*V2z, V1x*V2y-V1y*V2x).
    Pay attention that this operator is not conmutative. An exception is
    thrown if the vectors are not of the same shape or they don't belong
    to R3.
    \\ Ex:
    \begin{verbatim}
    matrix1D<T> X=vector_R3(1,0,0), Y=vector_R3(0,1,0);
    cout << "X*Y=Z=" << vector_product(X,Y).transpose() << endl;
    \end{verbatim}*/
template <class T> matrix1D<T> vector_product(const matrix1D<T> &v1, 
   const matrix1D<T> &v2) {
   if (XSIZE(v1)!=3 || XSIZE(v2)!=3)
      REPORT_ERROR(1002,"Vector_product: vectors are not in R3");
   if (v1.isRow()!=v2.isRow())
      REPORT_ERROR(1007,"Vector_product: vectors are of different shape");

   matrix1D<T> result(3);
   result.X()=v1.Y()*v2.Z()-v1.Z()*v2.Y();
   result.Y()=v1.Z()*v2.X()-v1.X()*v2.Z();
   result.Z()=v1.X()*v2.Y()-v1.Y()*v2.X();
   return result;
}

/** True if the three R3 vectors are orthogonal.
    Three R3 vectors are said to be orthogonal if the determinant of the
    matrix formed by the 3 is 0. An exception is thrown if the vectors
    do not belong to R3, but not if they don't have the same shape.
    \\ Ex:
    \begin{verbatim}
    matrix1D<double> X=vector_R3(1,0,0), Y=vector_R3(0,1,0), Z=vector_R3(0,0,1);
    if (are_orthogonal(X,Y,Z)) cout << "Of course, they are orthogonal\n";
    \end{verbatim}*/
int are_orthogonal(matrix1D<double> &v1, matrix1D<double> &v2,
    matrix1D<double> &v3);

/** True if the three R3 vectors form a coordinate system.
    Three R3 vectors form a coordinate system if they are orthogonal and
    V1*V2=V3, V2*V3=V1, and V3*V1=V2. The exact equality is checked in
    this function so there could be some numerical errors which cause
    the three vectors not to be recognized as a coordinate system.
    An exception is thrown if the vectors do not belong to R3.
    \\ Ex:
    \begin{verbatim}
    matrix1D<double> X=vector_R3(1,0,0), Y=vector_R3(0,1,0), Z=vector_R3(0,0,1);
    if (are_system(X,Y,Z))   cout << "Of course, they are a system\n";
    if (!are_system(-X,Y,Z)) cout << "But not in this case even if they are orthogonal\n";
    \end{verbatim}*/
int are_system(matrix1D<double> &v1, matrix1D<double> &v2,
    matrix1D<double> &v3);
//@}

/* Other useful functions -------------------------------------------------- */
/**@name Miscellaneous*/
//@{
/** Reduce both vectors to a common size.
    Search the range of logical indexes for which both vectors have got
    valid values, and cut both vectors to that size, the corresponding
    origin is automatically computed.
    \\Ex: matrix1D<double> v1(5); v1.startingX()=-2;
    \\\ \ \ \ matrix1D<double> v2(4); v2.startingX()=0;
    \\\ \ \ \ cut_to_common_size(v1,v2);
    \\--> v1 and v2 range from 0 to 2*/
template <class T>
   void cut_to_common_size(vT &V1, vT &V2) {
      int x0=MAX(STARTINGX(V1) ,STARTINGX(V2));
      int xF=MIN(FINISHINGX(V1),FINISHINGX(V2));
      V1.window(x0,xF);
      V2.window(x0,xF);
   }

/** Sort two vectors.
    v1 and v2 must be of the same shape, if not an exception is thrown.
    After calling this function all components in v1 are the minimum
    between the corresponding components in v1 and v2, and all components
    in v2 are the maximum.
    For instance, XX(v1)=MIN(XX(v1),XX(v2)), XX(v2)=MAX(XX(v1),XX(v2)).
    Notice that both vectors are modified. This function is very useful
    for sorting two corners. After calling it you can certainly perform
    a non-empty for (from corner1 to corner2) loop.*/
template <class T>
    void sort_two_vectors(vT &v1, vT &v2) {
      T temp;
      if (XSIZE(v1)!=XSIZE(v2) || STARTINGX(v1)!=STARTINGX(v2))
         REPORT_ERROR(1007, "sort_two_vectors: vectors are not of the same shape");
      FOR_ALL_ELEMENTS_IN_MATRIX1D(v1) {
         temp=MIN(VEC_ELEM(v1,i),VEC_ELEM(v2,i));
         VEC_ELEM(v2,i)=MAX(VEC_ELEM(v1,i),VEC_ELEM(v2,i));
         VEC_ELEM(v1,i)=temp;
      }
   }

/** Optimize using Powell's method.
    See Numerical Recipes Chapter 10.
    Problem: Minimize f(x) starting at point p. n is the dimension of x.
        If changes
        are smaller than ftol then stop. The number of iterations is
	returned in iter, fret contains the function value at the
	minimum and p contains the minimum.
   
    Watch out that the evaluating function f must consider that x starts
    at index 1, at least, and goes until n. i0 is used to allow optimization
    in large spaces where only one part is going to be optimized. Thus,
    if in a vector of dimension 20 you want to optimize the first 3 components
    then i0=1, n=3; if you want to optimize it all, i0=1, n=20; and if
    you want to optimize the last five components i0=15, n=5.
    
    The steps define the allowed steps on each variable. When a variable
    has large ranges this step should be larger to allow bigger movements.
    The steps should have only the dimension of the optimized part (3,20 and 5
    in the examples above).
    
    The option show forces the routine to show the convergence path */
void Powell_optimizer(matrix1D<double> &p, int i0, int n,
   double (*f)(double *x), double ftol, double &fret,
   int &iter, const matrix1D<double> &steps, bool show=false);
//@}
//@}
//@}

/* Implementations of common routines -------------------------------------- */
/* Print shape ------------------------------------------------------------- */
template <class T>
   void vT::print_shape(ostream &out) const {
   out << "Size: " << XSIZE(*this)
       << "i=[" << STARTINGX(*this) << ".." << FINISHINGX(*this) << "]";
}

/* Get size----------------------------------------------------------------- */
template <class T>
   void vT::get_size(int *size) const
   {size[0]=xdim; size[1]=1; size[2]=1;}

/* Outside ----------------------------------------------------------------- */
template <class T>
   bool vT::outside(const matrix1D<double> &v) const {
   if (XSIZE(v)<1)
      REPORT_ERROR(1,"Outside: index vector has got not enough components");
   return (XX(v)<STARTINGX(*this) || XX(v)>FINISHINGX(*this));
}

template <class T>
   bool vT::outside(int i) const {
   return (i<STARTINGX(*this) || i>FINISHINGX(*this));
}

/* Intersects -------------------------------------------------------------- */
template <class T>
   bool vT::intersects(const vT &m) const
      {return intersects(STARTINGX(m), XSIZE(m)-1);}

template <class T>
   bool vT::intersects(const matrix1D<double> &corner1,
      const matrix1D<double> &corner2) const {
       if (XSIZE(corner1)!=1 || XSIZE(corner2)!=1)
          REPORT_ERROR(1002,"intersects 1D: corner sizes are not 1");
       return intersects(XX(corner1),XX(corner2)-XX(corner1));
}

template <class T>
   bool vT::intersects(double x0, double xdim) const {
   SPEED_UP_temps;
   spduptmp0=MAX(STARTINGX(*this), x0);
   spduptmp1=MIN(FINISHINGX(*this),x0+xdim);
   if (spduptmp0>spduptmp1) return false;
   return true;
}

/* Corner ------------------------------------------------------------------ */
template <class T>
   bool vT::isCorner(const matrix1D<double> &v) {
   if (XSIZE(v)<1)
      REPORT_ERROR(1,"isCorner: index vector has got not enough components");
   return (XX(v)==STARTINGX(*this) || XX(v)==FINISHINGX(*this));
}

/* Border ------------------------------------------------------------------ */
template <class T>
   bool vT::isBorder(const matrix1D<int> &v) 
{
   if (XSIZE(v)<1)
      REPORT_ERROR(1,"isBorder: index vector has got not enough components");
   return  isBorder(XX(v));
}

template <class T>
   bool vT::isBorder(int i) 
{
   return (i==STARTINGX(*this)  || i==FINISHINGX(*this));
}

/* Patch ------------------------------------------------------------------- */
template <class T>
   void vT::patch(const vT &patch_array, char operation) {
      SPEED_UP_temps;
      FOR_ALL_ELEMENTS_IN_COMMON_IN_MATRIX1D(patch_array,*this)
         switch (operation) {
            case '=': VEC_ELEM(*this,i) =VEC_ELEM(patch_array,i); break;
            case '+': VEC_ELEM(*this,i)+=VEC_ELEM(patch_array,i); break;
            case '-': VEC_ELEM(*this,i)-=VEC_ELEM(patch_array,i); break;
            case '*': VEC_ELEM(*this,i)*=VEC_ELEM(patch_array,i); break;
            case '/': VEC_ELEM(*this,i)/=VEC_ELEM(patch_array,i); break;
         }
}

/* Output stream ----------------------------------------------------------- */
template <class T>
   ostream& operator << (ostream& out, const vT& v) {
   if (MULTIDIM_SIZE(v)==0) out << "NULL vector\n";
   else {
      // Look for the exponent
      vT aux(v); aux.ABSnD();
      int prec=best_prec(aux.compute_max(),10);

      FOR_ALL_ELEMENTS_IN_MATRIX1D(v) {      
         if (v.row) out << FtoA((double)VEC_ELEM(v,i),10,prec) << ' ';
         else       out << FtoA((double)VEC_ELEM(v,i),10,prec) << '\n';
      }
   }
      
   return out;
}

/* Statistics in region ---------------------------------------------------- */
template <class T>
void vT::compute_stats(double &avg, double &stddev, T &min_val, T &max_val,
   const matrix1D<double> &corner1, const matrix1D<double> &corner2) const {
   min_val=max_val=(*this)(corner1);
   matrix1D<double> r(3);
   double N=0, sum=0, sum2=0;
   FOR_ALL_ELEMENTS_IN_MATRIX1D_BETWEEN(corner1,corner2) {
      sum+=(*this)(r); sum2+=(*this)(r)*(*this)(r); N++;
      if      ((*this)(r)<min_val) min_val=(*this)(r);
      else if ((*this)(r)>max_val) max_val=(*this)(r);
   }
   if (N!=0) {
      avg=sum/N;
      stddev=sqrt(sum2/N-avg*avg);
   } else {avg=stddev=0;}
}

/* Minimum and maximum in region ------------------------------------------- */
template <class T>
void vT::compute_double_minmax(double &min_val, double &max_val,
   const matrix1D<double> &corner1, const matrix1D<double> &corner2) const {
   min_val=max_val=(*this)(corner1);
   matrix1D<double> r(1);
   FOR_ALL_ELEMENTS_IN_MATRIX1D_BETWEEN(corner1,corner2) {
      if      ((*this)(r)<min_val) min_val=(*this)(r);
      else if ((*this)(r)>max_val) max_val=(*this)(r);
   }
}

/* Center of mass ---------------------------------------------------------- */
template <class T>
   void vT::center_of_mass(matrix1D<double> &center, void * mask) {
      center.init_zeros(1);
      double mass=0;
      matrix1D<int> *imask=(matrix1D<int> *) mask;
      FOR_ALL_ELEMENTS_IN_MATRIX1D(*this) {
         if (imask==NULL || VEC_ELEM(*imask,i)) {
            XX(center)+=i*VEC_ELEM(*this,i);
	    mass+=VEC_ELEM(*this,i);
      	 }
      }
      if (mass!=0) center/=mass;
   }

#undef maT
#undef maT1
#endif
