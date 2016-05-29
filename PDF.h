/****************************************************************************/
/** 																	   **/
/** PDF.h - Front-end class for a PDF (Portable Document Format) File	   **/
/** 																	   **/
/** Copyright (C) 2014 Michael M. Stark, ALL RIGHTS RESERVED			   **/
/** 																	   **/
/**------------------------------------------------------------------------**/
/** Author:  Michael M. Stark											   **/
/****************************************************************************/

#ifndef __PDF_H
#define __PDF_H

#include <cstdlib>
#include <cstdio>
#define _USE_MATH_DEFINES
#include <cmath>
#include <cstring>

/*********/
/* Fonts */
/*********/

/* I'm not going to implement any but the built-in fonts */

// A font is an index into the 'FontNames' array

extern const char *FontNames[];

// These are the actual indices...

static const int Times_Roman = 0;
static const int Times       = 0;
static const int Times_Bold = 1;
static const int Times_Italic = 2;
static const int Times_BoldItalic = 3;

static const int Helvetica = 4;
static const int Helvetica_Bold = 5;
static const int Helvetica_Oblique = 6;
static const int Helvetica_BoldOblique = 7;

static const int Courier = 8;
static const int Courier_Bold = 9;
static const int Courier_Oblique = 10;
static const int Courier_BoldOblique = 11;

static const int Symbol = 12;
static const int ZapfDingbats = 16;

// But they can also be specified by the index of the base font
// combined with modifier flags ("Symbol" and "ZapfDingbats" have
// no other forms)

static const int BoldFlag    = 1;
static const int ItalicFlag  = 2;
static const int ObliqueFlag = 2;

inline int font_index( int font )
  // fixes the values for modified Symbol and ZapfDingbats
{
  if (font < 0)
    return 0;
  if (font >= 12 && font <= 15)
    return 12;
  if (font >= 16)
    return 13;
  return font;
}


/**************************************************************************** 
 * 
 * CLASS:  PDFColor
 * 
 ****************************************************************************/

class PDFColor {
 public:
  double r, g, b;

  static const PDFColor Black;
  
  PDFColor() { r = g = b = 0; }
  PDFColor( double gray ) { r = g = b = gray; }
  PDFColor( double red, double green, double blue ) {
    r = red;
    g = green;
    b = blue;
  }
};


/**************************************************************************** 
 * 
 * STRUCT:  PDFPoint
 * 
 ****************************************************************************/

struct PDFPoint {
  double x, y;

  PDFPoint() {}
  PDFPoint( double x, double y ) {
    this->x = x;
    this->y = y;
  }
  double length() const { return hypot(x, y); }
  double length_sqr() const { return x*x + y*y; }
  double dist( const PDFPoint& p ) const { return hypot(p.x - x, p.y - y); }
  double dist_sqr( const PDFPoint& p ) const {
    return (p.x - x)*(p.x - x) + (p.y - y)*(p.y - y);
  }
  double dot( const PDFPoint& p ) const {
    return (x*p.x + y*p.y);
  }
  double cross_z( const PDFPoint& p ) const {
    return (x*p.y - y*p.x);
  }
  PDFPoint perp() const { return PDFPoint(-y, x); }
  PDFPoint unit() const {
    const double s = 1/hypot(x, y);
    return PDFPoint(s*x, s*y);
  }
  PDFPoint operator+( const PDFPoint& p ) const {
    return PDFPoint(x + p.x, y + p.y);
  }
  PDFPoint operator-( const PDFPoint& p ) const {
    return PDFPoint(x - p.x, y - p.y);
  }
};

inline PDFPoint operator*( double s, const PDFPoint& p ) {
  return PDFPoint(s*p.x, s*p.y);
}
  


/**************************************************************************** 
 * 
 * CLASS:  PDFStream
 *
 ****************************************************************************/

/* This class implements a "stream" object for a PDF file.
 * One use of such a stream is to contain a sequence of graphics commands.
 * There is one 'PDFstream' per page in this implementation.
 */

// Forward declarations, for the "friend" class definitions
class PDF;  
class PDFPage;

class PDFStream {
 public:
  PDFStream() { init(); }
  ~PDFStream() { destroy(); }

  int is_empty() const { return (strlen(text) == 0); }
  void append( const char *src );

 private:
  
  char     *text;
  int       text_len;
  unsigned  size;

  void init( unsigned initial_size = 1024 );
  void grow( unsigned new_size = 0 );

  void destroy() { delete[] text; }
  
  friend class PDF;
  friend class PDFPage;
};


/**************************************************************************** 
 * 
 * CLASS:  PDFPage
 * 
 ****************************************************************************/

class PDFPage {
 public:
  PDFPage() { annotation = NULL; }
  ~PDFPage() { destroy(); }
  int is_empty() const { return stream.is_empty(); }
  
 private:
  PDFStream stream;
  char *annotation;

  void destroy() {
    if (annotation)
      free(annotation);
  }
  
  friend class PDF;
};


/**************************************************************************** 
 * 
 * CLASS:  PDF
 * 
 ****************************************************************************/

const int LetterWidth = int(72*8.5);
const int LetterHeight = int(72*11);

const int Backward = 1<<0;
const int Forward  = 1<<1;

class PDF {
 public:
  PDF( const char *filename,
       int width = LetterWidth, int height = LetterHeight ) {
    init(filename, width, height);
  }
  ~PDF() { destroy(); }

  /* Basic Operations */
  void new_page( const char *annotation = NULL );
  void title_page( const char *title, const char *subtitle = NULL,
		   const char *author = NULL,
		   int font = Helvetica_Bold, double font_scale = 24.0 );
  void finish();

  /* Size Accessors */
  int get_width() const { return width; }
  int get_height() const { return height; }

  /* Canvas to Device Transformation */
  PDFPoint transform( double x, double y ) {
    return PDFPoint(a11*x + a12*y + a13,
		    a21*x + a22*y + a13);
  }
  PDFPoint transform_vector( double x, double y ) {
    return PDFPoint(a11*x + a12*y,
		    a21*x + a22*y);
  }
  
  // Support stuff, for the page content
  void append( const char *cmd ) {
    pages[page].stream.append(cmd);
  }
  void cmd( const char *cmd ) {
    pages[page].stream.append(cmd);
  }
  void cmd( double v, const char *cmd ) {
    sprintf(buf, "%.3f %s", v, cmd);
    pages[page].stream.append(buf);
  }
  void cmd( double x, double y, const char *cmd ) {
    char buf[1024];
    sprintf(buf, "%.3f %.3f %s", x, y, cmd);
    append(buf);
  }  
  void int_cmd( int n, const char *cmd ) {
    sprintf(buf, "%d %s", n, cmd);
    pages[page].stream.append(buf);
  }
  void point_cmd( double x, double y, const char *cmd ) {
    PDFPoint p = transform(x, y);
    sprintf(buf, "%.3f %.3f %s", p.x, p.y, cmd);
    append(buf);
  }
  void point_cmd( double x1, double y1,
		  double x2, double y2,
		  double x3, double y3, const char *cmd ) {
    PDFPoint p1 = transform(x1, y1);
    PDFPoint p2 = transform(x2, y2);
    PDFPoint p3 = transform(x3, y3);    
    sprintf(buf, "%.3f %.3f %.3f %.3f %.3f %.3f %s",
	    p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, cmd);
    append(buf);
  }

  void comment( const char *src );
  
  
  /*****************/
  /* PDF Operators */
  /*****************/

  // 'b' closes the path, then fills and strokes
  // according to the current state
  void closepath_fill_stroke() { append("b"); current_point = 0; }
  void b() { append("b"); }

  // 'B' fills and strokes according to the current state  
  void fill_stroke() { append("B"); current_point = 0; }
  void B() { append("B"); }

  // 'b*' closes the path, then even/odd fills and strokes
  void closepath_eofill_stroke() { append("b*"); current_point = 0; }
  void b_star() { append("b*"); }

  // 'b*' even/odd fills and strokes  
  void eofill_stroke() { append("B*"); current_point = 0; }
  void B_star() { append("B*"); }

  // 'c' executes "curveto"
  void curveto( double x1, double y1,
		double x2, double y2,
		double x3, double y3 ) {
    point_cmd(x1, y1, x2, y2, x3, y3, "c");
    current_point = 1;
    current_x = x3;
    current_y = y3;
  }
  void c( double x1, double y1,
	  double x2, double y2,
	  double x3, double y3 ) {
    point_cmd(x1, y1, x2, y2, x3, y3, "c");
  }

  // 'cm' executes "concat", which combines the given matrix
  // with the current transformation matrix
  void concat( double x1, double y1,
	       double x2, double y2,
	       double x3, double y3 ) {
    sprintf(buf, "%.3f %.3f %.3f %.3f %.3f %.3f cm",
	    x1, y1, x2, y2, x3, y3);
    append(buf);
  }

  // 'd' executes "setdash"
  void setdash( double length, double offset = 0 ) {
    static char buf[64];
    sprintf(buf, "[ %.3f ] %.3f d", length, offset);
    append(buf);
  }
  void setdash( double length1, double length2, double offset = 0 ) {
    static char buf[64];
    sprintf(buf, "[ %.3f %.3f ] %.3f d", length1, length2, offset);
    append(buf);
  }
  void resetdash() { cmd("[] 0 d"); }

  // 'f' executes "fill"
  void fill() { cmd("f"); current_point = 0; }
  void f() { cmd("f"); }

  // 'f*' executes eofill (even-odd fill)
  void eofill() { cmd("f*"); current_point = 0; }
  void f_star() { cmd("f*"); }

  // 'G' sets the current graylevel for nonstroking operations
  void setgray_stroke( double gray ) {
    cmd(gray, "G");
    stroke_color = PDFColor(gray);
  }
  // 'q' sets the current graylevel for non-stroking operations  
  void setgray_nonstroke( double gray ) {
    cmd(gray, "g");
    nonstroke_color = PDFColor(gray);    
  }
  void setgray( double gray ) {
    setgray_stroke(gray);
    setgray_nonstroke(gray);
  }
  
  void G( double gray ) { cmd(gray, "G"); }
  void g( double gray ) { cmd(gray, "g"); }

  // 'h' closes the path
  void closepath() { cmd("h"); current_point = 0; }
  void h() { cmd("h"); }

  // 'i' sets the flatness
  //! NOTE: this is not right!
  void setflat() { cmd("i"); }
  void i() { cmd("i"); }

  // 'J' sets the line join
  void setlinejoin( int n ) { int_cmd(n, "J"); }
  void J( int n ) { int_cmd(n, "J"); }

  // 'j' sets the line cap
  void setlinecap( int n ) { cmd(n, "j"); }
  void j( int n ) { int_cmd(n, "j"); }

  // CMYK colors aren't complete; use 'setcolor' below
  void setcolor( double c, double m, double y, double k, const char *cmd ) {
    sprintf(buf, "%.3f %.3f %.3f %.3f %s", c, m, y, k, cmd);
    append(buf);
  }

  // 'K' sets the CMYK color for stroking operations  
  void setcmykcolor_stroke( double c, double m, double y, double k ) {
    setcolor(c, m, y, k, "K");
  }
  // 'k' sets the CMYK color for non-stroking operations  
  void setcmykcolor_nonstroke( double c, double m, double y, double k ) {
    setcolor(c, m, y, k, "k");
  }
  void k( double c, double m, double y, double k ) {
    setcolor(c, m, y, k, "k");
  }
  void K( double c, double m, double y, double k ) {
    setcolor(c, m, y, k, "K");
  }

  // 'l' is "lineto"
  void lineto( double x, double y ) {
    point_cmd(x, y, "l");
    current_x = x;
    current_y = y;
    current_point = 1;
  }
  void l( double x, double y )      { point_cmd(x, y, "l"); }

  // 'm' is "moveto"
  void moveto( double x, double y ) {
    point_cmd(x, y, "m");
    current_x = x;
    current_y = y;
    current_point = 1;

  }
  void m( double x, double y )      { point_cmd(x, y, "m"); }

  // 'M' sets the miter limit
  void setmiterlimit( double limit ) { cmd(limit, "M"); }
  void M( double limit ) { cmd(limit, "M"); }

  // 'n' ends the path, without drawing it
  void endpath() { cmd("n"); current_point = 0; }
  void n() { cmd("n"); }

  // 'q' executes "gsave"
  void gsave() { cmd("q"); }
  // 'Q' execures "grestore"
  void grestore() { cmd("Q"); }
  void q() { cmd("q"); }
  void Q() { cmd("Q"); }

  // 're' adds a rectangle to the path
  void rect( double x, double y, double width, double height ) {
    PDFPoint p = transform(x, y);
    PDFPoint v = transform_vector(width, height);
    sprintf(buf, "%.3f %.3f %.3f %.3f re", p.x, p.y, v.x, v.y);
    append(buf);
  }
  
  // "RG" sets the RGB color for stroking operations  
  void setrgbcolor_stroke( double r, double g, double b ) {
    setcolor(r, g, b, "RG");
    stroke_color = PDFColor(r, g, b);    
  }
  // "rg" sets the RGB color for non-stroking operations  
  void setrgbcolor_nonstroke( double r, double g, double b ) {
    setcolor(r, g, b, "rg");
    nonstroke_color = PDFColor(r, g, b);        
  }
  void setrgbcolor( double r, double g, double b ) {
    setrgbcolor_stroke(r, g, b);
    setrgbcolor_nonstroke(r, g, b);
  }
  void RG( double r, double g, double b ) { setcolor(r, g, b, "RG"); }
  void rg( double r, double g, double b ) { setcolor(r, g, b, "rg"); }

  // 's' closes the path then strokes
  void closepath_stroke() { cmd("s"); current_point = 0; }
  void s() { cmd("s"); }

  // 'S' strokes
  void stroke() { cmd("S"); current_point = 0; }
  void S() { cmd("S"); }

  // 'sh' executes "shfill" (shade fill)
  void shfill() { cmd("sh"); current_point = 0; }
  void sh()     { cmd("sh"); }

  // 'w' executes "setlinewidth"
  void setlinewidth( double width ) { cmd(width, "w"); }
  void w( double width ) { cmd(width, "w"); }

  // 'W' executes "clip"
  void clip() { cmd("W"); }
  void W() { cmd("W"); }

  // 'W*' executes "eoclip" (clip by even/odd rule)
  void eoclip() { cmd("W*"); }
  void W_star() { cmd("W*"); }

  /* Omitted:
   *
   * 'BDC' begin marked-context with property list
   * 'BI' begin inline image object
   * 'BMC' begin marked context
   * 'BX' begin compatibility section
   * 'CS' "setcolorspace" for stroking operations
   * 'cs' "setcolorspace" for non-stroking operations
   * 'd0' "setcharwidth" (set glyph width for Type 3 font)
   * 'd1' setcachedevice"
   * 'Do' invoke named XObject
   * 'DP' define marked-content point with property list
   * 'EI' end inline image object
   * 'EMC' end marked-content sequence
   * 'ET' end text object
   * 'EX' end compatibility section
   * 'gs' set parameters for graphics state dictionary
   * 'ID' begin inline image data
   * 'MP' defined marked-content point
   * 'ri' set color rendering intent
   * 'SC', 'sc' set color for stroking, nonstroking operations
   * 'SCN', 'scn' set color for stroking, nonstroking operations
   * 'TL' set text leading
   * 'Tm' set text and line matrix
   * 'Tr' set text rendering mode
   * 'Ts' set text rise
   * 'Tw' set word spacing
   * 'Tz' set horizontal text scaling
   */
  
  /*********/
  /* Color */
  /*********/

  void setcolor( double r, double g, double b, const char *cmd ) {
    sprintf(buf, "%.3f %.3f %.3f %s", r, g, b, cmd);
    append(buf);
  }
  void setcolor_stroke( const PDFColor& color ) {
    stroke_color = color;
    setrgbcolor_stroke(color.r, color.g, color.b);
  }
  void setcolor_nonstroke( const PDFColor& color ) {
    nonstroke_color = color;
    setrgbcolor_nonstroke(color.r, color.g, color.b);
  }
  void setcolor( const PDFColor& color ) {
    setcolor_stroke(color);
    setcolor_nonstroke(color);
  }


  /********/
  /* Text */
  /********/

  /* PDF Text objects are bounded by "BT" and "ET" operators.
   * There is a specific text matrix, and a text line matrix,
   * which are initialized to the identity matrices by the "BT"
   * operator (and discarded by the "ET" operator)
   */
  void begin_text() {
    cmd("BT");
    text = 1;
    selectfont(font, font_scale);
  }
  void end_text() {
    cmd("ET");
    text = 0;
  }
  void BT() { cmd("BT"); }
  void ET() { cmd("ET"); }

  /* The 'tx ty Td' operator moves to the next text line, offset from
   * the start of the current line by ('tx', 'ty')
   */
  void next_line( double tx, double ty ) { cmd(tx, ty, "Td"); }
  void Td( double tx, double ty )        { cmd(tx, ty, "Td"); }

  /* 'tx ty TD' moves to the next line as 'Td', but it has the side
   * effect of setting the leading (the inter-line spacing)
   */
  void next_line_leading( double tx, double ty ) { cmd(tx, ty, "TD"); }
  void TD( double tx, double ty )                { cmd(tx, ty, "TD"); }

  /* 'a b c d e f  Tm' sets the text matrix and the text line matrix:
   *
   *    | a b 0 |
   *    | c d 0 |
   *    | e f 1 |
   */
  void text_matrix( double a, double b,
		    double c, double d,
		    double e, double f ) {
    sprintf(buf, "%.4f %.4f %.4f %.4f %.4f %.4f Tm", a, b, c, d, e, f);
    append(buf);
  }
  void Tm( double a, double b,
	   double c, double d,
	   double e, double f ) {
    text_matrix(a, b, c, d, e, f);
  }

  /* 'T*' moves to the start of the next line */
  void next_line() { cmd("T*"); }
  void T_star()    { cmd("T*"); }


  /* Text-Showing Operations */

  /* 'string Tj' shows a text string */
  void show( const char *src, int n );
  void show( const char *src ) {
    show(src, strlen(src));
  }
  void Tj( const char *src ) {
    show(src, strlen(src));
  }

  /* "string '" moves to the next line and shows the string */
  void show_next_line( const char *src );

  /* 'w c string "' moves to the next line and shows the string,
   * with the side effect of setting the word spacing to 'w' and
   * the character spacing to 'c'.
   */
  void show_next_line( double w, double c, const char *src );

  /* 'array TJ' is a general operator for showing strings with
   *  additional spacing.  (It is omitted)
   */

  /*********/
  /* Fonts */
  /*********/

  void selectfont( int font, double scale );
  void Tf( int font, double scale ) {
    selectfont(font, scale);
  }


  /*****************************/
  /* More Complicated Graphics */
  /*****************************/

  void rmoveto( double x, double y );
  void rlineto( double x, double y );
 
  void arc( double x, double y, double r, double a0, double a1, int n = 0 );
  void arcn( double x, double y, double r, double a0, double a1, int n = 0 );
  void circle_path( double x, double y, double r );
  void rectpath( double x, double y, double width, double height );
  void round_box_path( double x, double y, double width, double height,
		       double r, int n_arc_segs = 1 );

  void polyline_path( int n, ... );
  void polygon_path( int n, ... );

  // Arrows
  void basic_arrowhead( double x, double y, double x0, double y0,
			double length, double width );
  void arrowed_line( double x0, double y0, double x1, double y1,
		     double length, double width, int heads = Forward,
		     double backward_offset = 0, double forward_offset = 0 );
  void arrowed_arc( double x, double y, double r, double a0, double a1,
		    double length, double width, int heads = Forward,
		    double a0_offset = 0, double a1_offset = 0 );
  void arrowed_arcn( double x, double y, double r, double a0, double a1,
		     double length, double width, int heads = Forward,
		     double a0_offset = 0, double a1_offset = 0 );
  
  // Text
  void position_text( const char *src,
		      double x, double y,
		      double h_frac = 0, double v_frac = 0 );
  void text_box( const char *src,
		 double x, double y, double margin, double r,
		 double min_width = 0, double min_height = 0 );

 protected:
  // Filename, etc
  char *filename;
  FILE *out;

  // Basic page dimensions
  int width, height;

  // Transformation
  double a11, a12, a13;
  double a21, a22, a23;
  
  // Page contents
  static const int max_pages = 1024;
  PDFPage pages[max_pages];
  int page; // current page index (starts at 0)

  // font vector (collection of document fonts)
  // 'fonts[k]' is set if font 'k' is used
  static const int max_fonts = 20;
  int fonts[max_fonts];

  // current font
  int    font;       // current font
  double font_scale; // current font scale
  int    text;       // true if we're in a text segment
  
  // colors, current point, etc.
  PDFColor stroke_color;
  PDFColor nonstroke_color;
  double   current_x;
  double   current_y;
  int      current_point;
  
  // character string buffer, for output
  static const unsigned buf_size = 4096;
  static char buf[buf_size];
  
  // Private Methods
  void init( const char *filename, int width, int height );
  void init_page();
  void finish_page();
  void destroy();
  void die( const char *msg );
};
  

#endif
