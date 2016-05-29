/****************************************************************************/
/** 																	   **/
/** PDF.cpp - Front-end class for a PDF (Portable Document Format) File	   **/
/** 																	   **/
/** Copyright (C) 2014 Michael M. Stark, ALL RIGHTS RESERVED			   **/
/** 																	   **/
/**------------------------------------------------------------------------**/
/** Author:  Michael M. Stark											   **/
/****************************************************************************/

#include <cstdlib>
#include <cstdio>
#include <cstring>
#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdarg>

#include "PDF.h"

/*********/
/* Fonts */
/*********/

const char *FontNames[] = {

  "Times-Roman",
  "Times-Bold",
  "Times-Italic",
  "Times-BoldItalic",

  "Helvetica",
  "Helvetica-Bold",
  "Helvetica-Oblique",
  "Helvetica-BoldOblique",

  "Courier",
  "Courier-Bold",
  "Courier-Oblique",
  "Courier-BoldOblique",

  "Symbol",

  "ZapfDingbats",
};

/***************************************/
/* Character Widths in the Basic Fonts */
/***************************************/

const double FontCharWidthScale = 1.0 / 1000.0;

short FontCharWidths[][256] = {

	/* Times-Roman */
	{499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,
	 499,499,499,499,499,499,499,499,499,499,499,499,499,247,331,406,499,499,829,
	 776,331,331,331,499,560,247,331,247,278,499,499,499,499,499,499,499,499,499,
	 499,278,278,560,560,560,441,918,719,666,666,719,609,556,719,719,331,388,719,
	 609,887,719,719,556,719,666,556,609,719,719,940,719,719,609,331,278,331,468,
	 499,331,441,499,441,499,441,331,499,499,278,278,499,278,776,499,499,499,499,
	 331,388,278,499,499,719,499,499,441,476,199,476,538,499,499,499,499,499,499,
	 499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,
	 499,499,499,499,499,499,499,499,499,331,499,499,163,499,499,499,499,176,441,
	 499,331,331,556,556,499,499,499,499,247,499,450,349,331,441,441,499,997,997,
	 499,441,499,331,331,331,331,331,331,331,331,499,331,331,499,331,331,331,997,
	 499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,887,499,274,
	 499,499,499,499,609,719,887,309,499,499,499,499,499,666,499,499,499,278,499,
	 499,278,499,719,499,499,499,499,499},

	 /* Times-Bold */
	 {499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,
	  499,499,499,499,499,499,499,499,499,499,499,499,499,247,331,551,499,499,997,
	  829,331,331,331,499,569,247,331,247,278,499,499,499,499,499,499,499,499,499,
	  499,331,331,569,569,569,499,926,719,666,719,719,666,609,776,776,388,499,776,
	  666,940,719,776,609,776,719,556,666,719,719,997,719,719,666,331,278,331,578,
	  499,331,499,556,441,556,441,331,499,556,278,331,556,278,829,556,499,556,556,
	  441,388,331,556,499,719,499,499,441,393,216,393,516,499,499,499,499,499,499,
	  499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,
	  499,499,499,499,499,499,499,499,499,331,499,499,163,499,499,499,499,278,499,
	  499,331,331,556,556,499,499,499,499,247,499,538,349,331,499,499,499,997,997,
	  499,499,499,331,331,331,331,331,331,331,331,499,331,331,499,331,331,331,997,
	  499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,997,499,300,
	  499,499,499,499,666,776,997,326,499,499,499,499,499,719,499,499,499,278,499,
	  499,278,499,719,556,499,499,499,499},

	  /* Times-Italic */
	  {499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,
	   499,499,499,499,499,499,499,499,499,499,499,499,499,247,331,419,499,499,829,
	   776,331,331,331,499,675,247,331,247,278,499,499,499,499,499,499,499,499,499,
	   499,331,331,675,675,675,499,918,609,609,666,719,609,609,719,719,331,441,666,
	   556,829,666,719,609,719,609,499,556,719,609,829,609,556,556,388,278,388,419,
	   499,331,499,499,441,499,441,278,499,499,278,278,441,278,719,499,499,499,499,
	   388,388,278,499,441,666,441,441,388,397,274,397,538,499,499,499,499,499,499,
	   499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,
	   499,499,499,499,499,499,499,499,499,388,499,499,163,499,499,499,499,212,556,
	   499,331,331,499,499,499,499,499,499,247,499,521,349,331,556,556,499,887,997,
	   499,499,499,331,331,331,331,331,331,331,331,499,331,331,499,331,331,331,887,
	   499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,887,499,274,
	   499,499,499,499,556,719,940,309,499,499,499,499,499,666,499,499,499,278,499,
	   499,278,499,666,499,499,499,499,499},

	   /* Times-BoldItalic */
	   {499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,
		499,499,499,499,499,499,499,499,499,499,499,499,499,247,388,551,499,499,829,
		776,331,331,331,499,569,247,331,247,278,499,499,499,499,499,499,499,499,499,
		499,331,331,569,569,569,499,829,666,666,666,719,666,666,719,776,388,499,666,
		609,887,719,719,609,719,666,556,609,719,666,887,666,609,609,331,278,331,569,
		499,331,499,499,441,499,441,331,499,556,278,278,499,278,776,556,499,499,499,
		388,388,278,556,441,666,499,441,388,344,216,344,569,499,499,499,499,499,499,
		499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,
		499,499,499,499,499,499,499,499,499,388,499,499,163,499,499,499,499,278,499,
		499,331,331,556,556,499,499,499,499,247,499,499,349,331,499,499,499,997,997,
		499,499,499,331,331,331,331,331,331,331,331,499,331,331,499,331,331,331,997,
		499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,940,499,265,
		499,499,499,499,609,719,940,300,499,499,499,499,499,719,499,499,499,278,499,
		499,278,499,719,499,499,499,499,499},

		/* Helvetica */
		{278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,
		 278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,353,556,556,887,
		 666,221,331,331,388,582,278,331,278,278,556,556,556,556,556,556,556,556,556,
		 556,278,278,582,582,582,556,1015,666,666,719,719,666,609,776,719,278,499,
		 666,556,829,719,776,666,776,719,666,609,719,666,940,666,666,609,278,278,278,
		 468,556,221,556,556,499,556,556,278,556,556,221,221,499,221,829,556,556,556,
		 556,331,499,278,556,499,719,499,499,499,331,256,331,582,278,278,278,278,278,
		 278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,
		 278,278,278,278,278,278,278,278,278,278,331,556,556,163,556,556,556,556,190,
		 331,556,331,331,499,499,278,556,556,556,278,278,534,349,221,331,331,556,997,
		 997,278,609,278,331,331,331,331,331,331,331,331,278,331,331,278,331,331,331,
		 997,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,997,278,
		 366,278,278,278,278,556,776,997,362,278,278,278,278,278,887,278,278,278,278,
		 278,278,221,609,940,609,278,278,278,278},

		 /* Helvetica-Bold */
		 {499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,
		  499,499,499,499,499,499,499,499,499,499,499,499,499,278,331,472,556,556,887,
		  719,278,331,331,388,582,278,331,278,278,556,556,556,556,556,556,556,556,556,
		  556,331,331,582,582,582,609,975,719,719,719,719,666,609,776,719,278,556,719,
		  609,829,719,776,666,776,719,666,609,719,666,940,666,666,609,331,278,331,582,
		  556,278,556,609,556,609,556,331,609,609,278,278,556,278,887,609,609,609,609,
		  388,556,331,609,556,776,556,556,499,388,278,388,582,499,499,499,499,499,499,
		  499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,
		  499,499,499,499,499,499,499,499,499,331,556,556,163,556,556,556,556,234,499,
		  556,331,331,609,609,499,556,556,556,278,499,556,349,278,499,499,556,997,997,
		  499,609,499,331,331,331,331,331,331,331,331,499,331,331,499,331,331,331,997,
		  499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,997,499,366,
		  499,499,499,499,609,776,997,362,499,499,499,499,499,887,499,499,499,278,499,
		  499,278,609,940,609,499,499,499,499},

		  /* Helvetica-Oblique */
		  {499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,
		   499,499,499,499,499,499,499,499,499,499,499,499,499,278,278,353,556,556,887,
		   666,221,331,331,388,582,278,331,278,278,556,556,556,556,556,556,556,556,556,
		   556,278,278,582,582,582,556,1015,666,666,719,719,666,609,776,719,278,499,
		   666,556,829,719,776,666,776,719,666,609,719,666,940,666,666,609,278,278,278,
		   468,556,221,556,556,499,556,556,278,556,556,221,221,499,221,829,556,556,556,
		   556,331,499,278,556,499,719,499,499,499,331,256,331,582,499,499,499,499,499,
		   499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,
		   499,499,499,499,499,499,499,499,499,499,331,556,556,163,556,556,556,556,190,
		   331,556,331,331,499,499,499,556,556,556,278,499,534,349,221,331,331,556,997,
		   997,499,609,499,331,331,331,331,331,331,331,331,499,331,331,499,331,331,331,
		   997,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,997,499,
		   366,499,499,499,499,556,776,997,362,499,499,499,499,499,887,499,499,499,278,
		   499,499,221,609,940,609,499,499,499,499},

		   /* Helvetica-BoldOblique */
		   {499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,
			499,499,499,499,499,499,499,499,499,499,499,499,499,278,331,472,556,556,887,
			719,278,331,331,388,582,278,331,278,278,556,556,556,556,556,556,556,556,556,
			556,331,331,582,582,582,609,975,719,719,719,719,666,609,776,719,278,556,719,
			609,829,719,776,666,776,719,666,609,719,666,940,666,666,609,331,278,331,582,
			556,278,556,609,556,609,556,331,609,609,278,278,556,278,887,609,609,609,609,
			388,556,331,609,556,776,556,556,499,388,278,388,582,499,499,499,499,499,499,
			499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,
			499,499,499,499,499,499,499,499,499,331,556,556,163,556,556,556,556,234,499,
			556,331,331,609,609,499,556,556,556,278,499,556,349,278,499,499,556,997,997,
			499,609,499,331,331,331,331,331,331,331,331,499,331,331,499,331,331,331,997,
			499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,997,499,366,
			499,499,499,499,609,776,997,362,499,499,499,499,499,887,499,499,499,278,499,
			499,278,609,940,609,499,499,499,499},

			/* Courier */
			{600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			 600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			 600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			 600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			 600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			 600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			 600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			 600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			 600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			 600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			 600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			 600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			 600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			 600,600,600,600,600,600,600,600,600},

			 /* Courier-Bold */
			 {600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			  600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			  600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			  600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			  600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			  600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			  600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			  600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			  600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			  600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			  600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			  600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			  600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			  600,600,600,600,600,600,600,600,600},

			  /* Courier-Oblique */
			  {600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			   600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			   600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			   600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			   600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			   600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			   600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			   600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			   600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			   600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			   600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			   600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			   600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
			   600,600,600,600,600,600,600,600,600},

			   /* Courier-BoldOblique */
			   {499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,
				499,499,499,499,499,499,499,499,499,499,499,499,499,600,600,600,600,600,600,
				600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
				600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
				600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
				600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,
				600,600,600,600,600,600,600,600,600,600,600,600,600,499,499,499,499,499,499,
				499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,
				499,499,499,499,499,499,499,499,499,600,600,600,600,600,600,600,600,600,600,
				600,600,600,600,600,499,600,600,600,600,499,600,600,600,600,600,600,600,600,
				499,600,499,600,600,600,600,600,600,600,600,499,600,600,499,600,600,600,600,
				499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,499,600,499,600,
				499,499,499,499,600,600,600,600,499,499,499,499,499,600,499,499,499,600,499,
				499,600,600,600,600,499,499,499,499},

				/* Symbol */
				{247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,
				 247,247,247,247,247,247,247,247,247,247,247,247,247,247,331,710,499,547,829,
				 776,437,331,331,499,547,247,547,247,278,499,499,499,499,499,499,499,499,499,
				 499,278,278,547,547,547,441,547,719,666,719,609,609,759,600,719,331,631,719,
				 684,887,719,719,768,737,556,591,609,688,437,768,644,794,609,331,860,331,657,
				 499,499,631,547,547,490,437,521,410,600,326,600,547,547,574,521,547,547,521,
				 547,600,437,574,710,684,490,684,490,476,199,476,547,247,247,247,247,247,247,
				 247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,247,
				 247,247,247,247,247,247,247,247,759,618,243,547,163,710,499,750,750,750,750,
				 1041,984,600,984,600,397,547,410,547,547,710,490,459,547,547,547,547,997,
				 600,997,657,821,684,794,984,768,768,821,768,768,710,710,710,710,710,710,710,
				 768,710,790,790,887,821,547,247,710,600,600,1041,984,600,984,600,490,326,
				 790,790,785,710,384,384,384,384,384,384,490,490,490,490,247,326,274,684,684,
				 684,384,384,384,384,384,384,490,490,490,247},

				 /* ZapfDingbats */
				 {278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,
				  278,278,278,278,278,278,278,278,278,278,278,278,278,278,971,957,971,979,715,
				  785,790,790,688,957,935,547,851,909,931,909,944,971,754,843,759,759,569,675,
				  759,759,759,750,490,551,534,574,688,785,785,785,790,790,790,812,821,785,838,
				  821,829,812,829,922,741,719,746,790,790,693,772,768,790,759,706,706,679,697,
				  825,812,785,785,706,684,693,688,785,785,710,790,781,790,869,759,759,759,759,
				  759,891,891,785,781,437,137,274,415,388,388,666,666,278,278,278,278,278,278,
				  278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,
				  278,278,278,278,278,278,278,278,278,728,543,543,909,666,759,759,772,591,693,
				  622,785,785,785,785,785,785,785,785,785,785,785,785,785,785,785,785,785,785,
				  785,785,785,785,785,785,785,785,785,785,785,785,785,785,785,785,785,785,785,
				  785,785,785,891,834,1015,454,746,922,746,918,926,926,926,834,869,825,922,
				  922,913,926,931,459,882,834,834,865,865,693,693,874,278,874,759,944,768,865,
				  768,887,966,887,829,869,926,966,918,278},
};

/****************/
/* Static Stuff */
/****************/

const PDFColor PDFColor::Black(0);

/****************************************************************************/
/***                      PDFStream Implementation			  ***/
/****************************************************************************/

void PDFStream::append(const char *src)
{
	const int src_len = strlen(src);
	while (unsigned(text_len + src_len) >= size - 2)
		grow();
	strcat(text + text_len, src);
	text_len += src_len;
	strcat(text + text_len, "\n");
	text_len++;
}

void PDFStream::init(unsigned initial_size)
{
	size = initial_size;
	text = new char[initial_size];
	strcpy(text, "");
	text_len = 0;
}

void  PDFStream::grow(unsigned new_size)
{
	if (new_size == 0)
		new_size = 2 * size;
	if (new_size < size)
		return;
	char *new_text = new char[new_size];
	strcpy(new_text, text);
	delete[] text;
	text = new_text;
	size = new_size;
}


/****************************************************************************/
/***                          PDF Implementation			  ***/
/****************************************************************************/

char PDF::buf[];  // (define the output buffer)

void PDF::init(const char *filename, int width, int height)
{
	this->filename = _strdup(filename);
	page = 0;
	this->width = width;
	this->height = height;

	// set the font vector to all false
	for (int k = 0; k < max_fonts; k++)
		fonts[k] = 0;

	// just in case, set the current font
	font = Times;
	font_scale = 1;

	// initialize the colors
	stroke_color = PDFColor(0);
	nonstroke_color = PDFColor(0);

	init_page();
}

void PDF::init_page()
{
	text = 0;
	current_point = 0;
	a11 = 1;  a12 = 0;  a13 = 0;
	a21 = 0;  a22 = 1;  a23 = 0;
}

void PDF::finish_page()
{
	// write in the annotation, at the top left
	if (pages[page].annotation) {
		selectfont(Helvetica | ObliqueFlag, 12);
		setcolor_nonstroke(PDFColor(0));
		position_text(pages[page].annotation, 72, height - 72 - 12);
	}
}

void PDF::new_page(const char *annot)
{
	// Check for the first page
	if (page == 0 && pages[0].is_empty()) {
		// don't start a new page
	}
	else {
		// Otherwise, finish the old page increment 'page'
		finish_page();
		page++;
		if (page >= max_pages) {
			fprintf(stderr, "Too many pages!\n");
			exit(1);
		}
		init_page();
	}

	// set the annotation
	if (annot) {
		pages[page].annotation = _strdup(annot);
	}
}


void PDF::destroy()
{
	free(filename);
}

void PDF::die(const char *msg)
{
	fprintf(stderr, "PDF error: %s\n", msg);
	exit(1);
}

/***********************/
/* Other Functionality */
/***********************/

void PDF::show(const char *src, int n)
{
	if (n + 6 >= (int)buf_size) {
		fprintf(stderr, "String too long\n");
		exit(1);
	}
	buf[0] = '(';
	for (int k = 0; k < n; k++)
		buf[k + 1] = src[k];
	sprintf(buf + n + 1, ") Tj");
	append(buf);
}

void PDF::show_next_line(const char *src)
{
	if (strlen(src) + 6 >= buf_size) {
		fprintf(stderr, "String too long\n");
		exit(1);
	}
	sprintf(buf, "(%s) '", src);
	append(buf);
}

void PDF::show_next_line(double w, double c, const char *src)
{
	if (strlen(src) + 40 >= buf_size) {
		fprintf(stderr, "String too long\n");
		exit(1);
	}
	sprintf(buf, "%.4f %.4f (%s) \"", w, c, src);
	append(buf);
}

void PDF::selectfont(int font, double scale)
{
	// ensure the font (index) is in the proper range
	font = font_index(font);

	// set the current font
	this->font = font;
	this->font_scale = scale;

	// mark the font in the font vector (so that a font dictionary
	// can be included)
	fonts[this->font] = 1;

	// if we're in a text segment, apply the command
	if (text) {
		sprintf(buf, "/F%d %.6f Tf", this->font, this->font_scale);
		append(buf);
	}
}


/******************/
/* Graphics Stuff */
/******************/

double Bezier_s(double angle)
// For part of an arc
{
	return (8 * cos(angle / 2) - 4 * (1 + cos(angle))) / (3 * sin(angle));
}

void PDF::arc(double x, double y, double r, double a0, double a1, int n)
// NOTE: Angles are in radians
{
	while (a1 < a0)
		a1 += 2 * M_PI;

	// to match the behavior of the PostScript operator (sort of)
	if (current_point)
		lineto(x + r*cos(a0), y + r*sin(a0));
	else
		moveto(x + r*cos(a0), y + r*sin(a0));

	// subdivide so that the segments are sufficiently small
	if (n == 0)
		n = int((a1 - a0) / (M_PI / 8));
	if (n < 1)
		n = 1;
	for (int k = 0; k < n; k++) {
		double t = double(k) / double(n);
		double rot = a0 + t*(a1 - a0);
		double angle = (a1 - a0) / double(n);
		double s = r*Bezier_s(angle);

		double x1 = r;
		double y1 = s;
		double x2 = r*cos(angle) + s*sin(angle);
		double y2 = r*sin(angle) - s*cos(angle);
		double x3 = r*cos(angle);
		double y3 = r*sin(angle);

		curveto(x + (x1*cos(rot) - y1*sin(rot)),
			y + (x1*sin(rot) + y1*cos(rot)),
			x + (x2*cos(rot) - y2*sin(rot)),
			y + (x2*sin(rot) + y2*cos(rot)),
			x + (x3*cos(rot) - y3*sin(rot)),
			y + (x3*sin(rot) + y3*cos(rot)));

	}
}

void PDF::arcn(double x, double y, double r, double a0, double a1, int n)
// NOTE: Angles are in radians
{
	while (a1 > a0)
		a1 -= 2 * M_PI;

	if (current_point)
		lineto(x + r*cos(a0), y + r*sin(a0));
	else
		moveto(x + r*cos(a0), y + r*sin(a0));

	// subdivide so that the segments are sufficiently small
	if (n == 0)
		n = int((a0 - a1) / (M_PI / 8));
	if (n < 1)
		n = 1;
	for (int k = 0; k < n; k++) {
		double t = double(k) / double(n);
		double rot = a0 + t*(a1 - a0);
		double angle = (a0 - a1) / double(n);  // angle is positive
		double s = r*Bezier_s(angle);

		double x1 = r;
		double y1 = -s;
		double x2 = r*cos(-angle) + s*sin(angle);
		double y2 = r*sin(-angle) + s*cos(angle);
		double x3 = r*cos(-angle);
		double y3 = r*sin(-angle);

		curveto(x + (x1*cos(rot) - y1*sin(rot)),
			y + (x1*sin(rot) + y1*cos(rot)),
			x + (x2*cos(rot) - y2*sin(rot)),
			y + (x2*sin(rot) + y2*cos(rot)),
			x + (x3*cos(rot) - y3*sin(rot)),
			y + (x3*sin(rot) + y3*cos(rot)));

	}
}

void PDF::rectpath(double x, double y, double width, double height)
{
	moveto(x, y);
	lineto(x + width, y);
	lineto(x + width, y + height);
	lineto(x, y + height);
	closepath();
}

void PDF::round_box_path(double x, double y, double width, double height,
	double r, int n_arc_segs)
{
	if (r == 0)
		rectpath(x, y, width, height);
	else {
		moveto(x + r, y);
		arc(x + width - r, y + r, r, -M_PI / 2, 0, n_arc_segs);
		arc(x + width - r, y + height - r, r, 0, M_PI / 2, n_arc_segs);
		arc(x + r, y + height - r, r, M_PI / 2, M_PI, n_arc_segs);
		arc(x + r, y + r, r, M_PI, 3 * M_PI / 2, n_arc_segs);
		closepath();
	}
}


/***********************/
/* More Graphics Stuff */
/***********************/

void PDF::rmoveto(double x, double y) {
	if (!current_point)
		die("no current point in 'rmoveto'");
	current_x += x;
	current_y += y;
	point_cmd(x, y, "m");
}

void PDF::rlineto(double x, double y) {
	if (!current_point)
		die("no current point in 'line'");
	current_x += x;
	current_y += y;
	point_cmd(x, y, "l");
}


void PDF::polyline_path(int n, ...)
{
	va_list args;
	va_start(args, n);
	if (n <= 0)
		return;

	double x, y;
	x = va_arg(args, double);
	y = va_arg(args, double);
	moveto(x, y);

	for (int k = 1; k < n; k++) {
		x = va_arg(args, double);
		y = va_arg(args, double);
		lineto(x, y);
	}
}

void PDF::polygon_path(int n, ...)
{
	va_list args;
	va_start(args, n);
	if (n <= 0)
		return;

	double x, y;
	x = va_arg(args, double);
	y = va_arg(args, double);
	moveto(x, y);

	for (int k = 1; k < n; k++) {
		x = va_arg(args, double);
		y = va_arg(args, double);
		lineto(x, y);
	}
	closepath();
}


void PDF::circle_path(double x, double y, double r)
{
	arc(x, y, r, 0, 2 * M_PI, 8);
	closepath();
}

/**************************/
/* Text, with Positioning */
/**************************/

int count_lines(const char *text)
{
	int n_lines = 1;
	const char *ptr = strchr(text, '\n');
	while (ptr) {
		n_lines++;
		ptr = strchr(ptr + 1, '\n');
	}
	return n_lines;
}

double stringwidth(const char *text, int n, int font, double scale)
{
	font = font_index(font); // be sure it's a valid index for the widths
	double width = 0;
	for (int k = 0; k < n && text[k]; k++)
		width += FontCharWidths[font][(unsigned)text[k]] * FontCharWidthScale;
	return scale*width;
}

double stringwidth_multiline(const char *text, int font, double scale)
{
	font = font_index(font); // be sure it's a valid index for the widths
	double max_width = 0;
	const char* ptr = text;
	while (*ptr) {
		// determine the end of the line in 'ptr'
		const char *end = strchr(ptr, '\n');
		if (!end)
			end = ptr + strlen(ptr);
		double width = stringwidth(ptr, end - ptr, font, scale);
		if (width >= max_width)
			max_width = width;
		ptr = (*end ? end + 1 : end);
	}

	return max_width;
}


void PDF::position_text(const char *text,
	double x, double y,
	double h_frac, double v_frac)
	// Positions 'text' at (x, y) according to 'h_frac' and 'v_frac'
	// of the bounding box.  If 'h_frac' is zero, the text is positioned
	// so that the left edge hits x; if it is 0.5, the text is centered
	// at x; if it is 1.0, the right edge hits 'x'.
	// This works with multiline strings.
{
	double leading = font_scale; // pretty much minimial...
	double em = 0.66667*font_scale; // approximate

	// compute the height
	int n_lines = count_lines(text);
	double height = (n_lines - 1)*leading + em;

	// Start a new text object (this also selects the current font)
	begin_text();

	// move to the position
	next_line(x, y);

	double ty = (n_lines - 1)*leading + v_frac*height;
	next_line(0, -ty); // (vertical shift only)

	// now split the string by lines and draw them
	const char *ptr = text;
	while (*ptr) {
		// determine the end of the line in 'ptr'
		const char *end = strchr(ptr, '\n');
		if (!end)
			end = ptr + strlen(ptr);
		double width = stringwidth(ptr, end - ptr, font, font_scale);
		if (h_frac != 0)
			next_line(-width*h_frac, 0);
		show(ptr, end - ptr);
		if (h_frac != 0)
			next_line(width*h_frac, 0);
		next_line(0, -leading);

		ptr = (*end ? end + 1 : end);
	}

	// End the text object
	end_text();
}


void PDF::text_box(const char *text, double x, double y,
	double margin, double r,
	double min_width, double min_height)
{
	double leading = font_scale; // pretty much minimial...
	double em = 0.66667*font_scale; // approximate

	// compute the width and height
	double width = stringwidth_multiline(text, font, font_scale);
	double height = (count_lines(text) - 1)*leading + em;
	width = (width < min_width ? min_width : width);
	height = (height < min_height ? min_height : height);

	width += 2 * margin;
	height += 2 * margin;

	// fill the box
	round_box_path(x - width / 2, y - height / 2, width, height, r);
	fill();

	// draw the text
	// (text is painted in the "nonstroke" color, but that's not what
	// we want here)
	PDFColor color0 = nonstroke_color;
	setcolor_nonstroke(stroke_color);
	position_text(text, x, y, 0.5, 0.5);
	setcolor_nonstroke(color0);

	// outline the box
	round_box_path(x - width / 2, y - height / 2, width, height, r);
	stroke();
}


/***************************/
/* Title Page and Headings */
/***************************/

void PDF::title_page(const char *title, const char *subtitle,
	const char *author, int font, double font_scale)
{
	// start a new page
	new_page();

	double x_mid = width / 2;
	double y = height / 2 + 144;

	if (title) {
		selectfont(font, font_scale);
		position_text(title, x_mid, y, 0.5, 0.0);
		y -= font_scale*count_lines(title) + 20;
	}

	if (subtitle) {
		selectfont(font, font_scale*0.707);
		position_text(subtitle, x_mid, y, 0.5, 0.0);
		y -= font_scale*count_lines(subtitle) + 20;
	}

	if (author) {
		selectfont(font, font_scale*0.5);
		position_text(author, x_mid, y, 0.5, 0.0);
		y -= font_scale*count_lines(author) + 20;
	}
}


/************/
/* Comments */
/************/

void PDF::comment(const char *src)
{
	const char* ptr = src;
	while (*ptr) {
		// determine the end of the line in 'ptr'
		const char *end = strchr(ptr, '\n');
		if (!end)
			end = ptr + strlen(ptr);
		double len = end - ptr;
		if (len >= buf_size - 2) {
			fprintf(stderr, "Comment string too long\n");
		}
		else {
			// write the string to 'buf', preceeded by "%"
			buf[0] = '%';
			for (const char *p = ptr; p < end; p++)
				buf[p - ptr + 1] = *p;
			buf[end - ptr + 1] = '\0';
			append(buf);
		}
		ptr = (*end ? end + 1 : end);
	}
}


/**********/
/* Arrows */
/**********/

void PDF::basic_arrowhead(double x, double y, double x0, double y0,
	double length, double width)
	// Draws a basic triangular arrowhead with point at (x, y), as if
	// it lay on the line originating fomr (x0, y0).
	// 'length' is the length of the arrowhead; 'widht' is the width
{
	double angle = atan2(y0 - y, x0 - x);
	PDFPoint pts[3];
	pts[0] = PDFPoint(0, 0);
	pts[1] = PDFPoint(length, -width / 2);
	pts[2] = PDFPoint(length, width / 2);
	for (int k = 0; k < 3; k++) {
		double tx = x + cos(angle)*pts[k].x - sin(angle)*pts[k].y;
		double ty = y + sin(angle)*pts[k].x + cos(angle)*pts[k].y;
		if (k == 0)
			moveto(tx, ty);
		else
			lineto(tx, ty);
	}
	closepath();
	fill();
}

void PDF::arrowed_line(double x0, double y0, double x1, double y1,
	double length, double width, int heads,
	double backward_offset, double forward_offset)
{
	double dx = x1 - x0;
	double dy = y1 - y0;
	double len = hypot(dx, dy);
	dx /= len;
	dy /= len;
	double offset = 0.3*length;

	// adjust the points by the offsets
	x0 += dx*backward_offset;
	y0 += dy*backward_offset;

	x1 -= dx*forward_offset;
	y1 -= dy*forward_offset;

	// if there is a backward arrowhead, adjust the line start point
	if (heads & Backward)
		moveto(x0 + dx*offset, y0 + dy*offset);
	else
		moveto(x0, y0);

	// if there is forward arrowhead, adjust the line end point
	if (heads & Forward)
		lineto(x1 - dx*offset, y1 - dy*offset);
	else
		lineto(x1, y1);
	stroke();

	// draw the arrowheads
	if (heads & Backward)
		basic_arrowhead(x0, y0, x1, y1, length, width);
	if (heads & Forward)
		basic_arrowhead(x1, y1, x0, y0, length, width);
}

void PDF::arrowed_arc(double x, double y, double r, double a0, double a1,
	double length, double width, int heads,
	double a0_offset, double a1_offset)
{
	double offset = 0.6*length / r;

	// adjust the angular boundaries by the offsets
	a0 += a0_offset / r;
	a1 -= a1_offset / r;

	// draw the arc, with appropriate offsets
	arc(x, y, r,
		a0 + (heads & Backward ? offset : 0),
		a1 - (heads & Forward ? offset : 0),
		(fabs(a1 - a0) < M_PI / 2 ? 1 : 0));
	stroke();

	// draw the arrowheads
	if (heads & Backward) {
		double x1 = x + r*cos(a0);
		double y1 = y + r*sin(a0);
		basic_arrowhead(x1, y1, x1 + cos(a0 + M_PI / 2), y1 + sin(a0 + M_PI / 2),
			length, width);
	}
	if (heads & Forward) {
		double x1 = x + r*cos(a1);
		double y1 = y + r*sin(a1);
		basic_arrowhead(x1, y1, x1 + cos(a1 - M_PI / 2), y1 + sin(a1 - M_PI / 2),
			length, width);
	}
}


void PDF::arrowed_arcn(double x, double y, double r, double a0, double a1,
	double length, double width, int heads,
	double a0_offset, double a1_offset)
{
	double offset = 0.6*length / r;

	// adjust the angular boundaries by the offsets
	a0 -= a0_offset / r;
	a1 += a1_offset / r;

	// draw the arc, with appropriate offsets
	arcn(x, y, r,
		a0 - (heads & Backward ? offset : 0),
		a1 + (heads & Forward ? offset : 0),
		(fabs(a1 - a0) < M_PI / 2 ? 1 : 0));
	stroke();

	// draw the arrowheads
	if (heads & Backward) {
		double x1 = x + r*cos(a0);
		double y1 = y + r*sin(a0);
		basic_arrowhead(x1, y1, x1 + cos(a0 - M_PI / 2), y1 + sin(a0 - M_PI / 2),
			length, width);
	}
	if (heads & Forward) {
		double x1 = x + r*cos(a1);
		double y1 = y + r*sin(a1);
		basic_arrowhead(x1, y1, x1 + cos(a1 + M_PI / 2), y1 + sin(a1 + M_PI / 2),
			length, width);
	}
}


/****************************************************************************/
/***                               Output				  ***/
/****************************************************************************/

/* Here is how the objects are arranged in the output.
   Assuming there are 'n' pages and a total of 'm' fonts

%PDF1-4

1 0 obj
  << /Type /Catalog   /Outlines 2 0 R    /Pages 3 0 R   >>
endobj

2 0 obj
  << /Type Outlines   /Count 0  >>
endobj

3 0 obj
  << /Type /Pages
	 /Kids [ 4 0 R ... ]  % (runs from 4 to 4 + 'n' - 1)
	 /Count <number-of-pages>
  >>
endobj

% The individual pages start at object 4; so page 'k' is

'4 + k' 0 obj
<< /Type /Page
  /Parent 3 0 R  % (the same for every page)
  /MediaBox [ 0 0 'width' 'height' ]
  /Contents << '4 + n + m + 2*(k - 1)' 0 R
  /Resources << /ProcSet '4 + n + m + 2*(k - 1) + 1' 0 R
				/Font << <fonts> >>
			 >>
 >>
endobj

% After all 'n' page objects comes the font objects
% There is one of each document font; font 'i' looks like this
% (where 'index' is the index of the font in the 'FontNames' array)

'4 + n + i' 0 obj
<< /Type /Font
 /Subtype /Type1
 /Name /F'index'
 /BaseFont 'name'
 /Encoding /MacRomanEncoding
>>
endobj


% Next is the content stream for the pages, paired with the
% procset for the page.  Page 'k' thus has
'4 + n + m + 2*(k - 1)' 0 obj
  << /Length  >>
stream
...
endstream
endobj

'4 + n + m + 2*(k - 1) + 1' 0 obj
  [/PDF /Text]
endobj

% Then comes the xref (cross references) section, the trailer, etc.

*/


void PDF::finish()
{
	// finish the current page
	finish_page();

	// Open the output file
	out = fopen(filename, "w");
	if (!out) {
		fprintf(stderr, "Can't write to '%s'\n", filename);
		exit(1);
	}

	// page count
	int n_pages = page + 1;

	// compute the total number of fonts used
	// (a separate Font object is needed for each base font)
	int font_count = 0;
	for (int k = 0; k < max_fonts; k++)
		if (fonts[k])
			font_count++;

	// 'objects' contains the text (literally) of each of the objects
	const int max_objects = 3 * n_pages + 16 + font_count;
	char **objects = new char*[max_objects];

	// For convenience, let 'objects[0]' be the header
	// (that way the index in the 'objects' array matches the
	// index of the object in the PDF file
	objects[0] = new char[16];
	sprintf(objects[0], "%%PDF-1.4\n\n");

	// The first object is the "Catalog"
	// It refers to the "Outlines" object (object 2) and the
	// "Pages" object (object 3)
	objects[1] = new char[256];
	sprintf(objects[1], "1 0 obj\n"
		"  << /Type /Catalog\n"
		"     /Outlines 2 0 R\n"
		"     /Pages 3 0 R\n"
		"  >>\n"
		"endobj\n\n");

	// The next object is the "Outlines" object (of which there are none)
	objects[2] = new char[256];
	sprintf(objects[2], "2 0 obj\n"
		"  << /Type Outlines\n"
		"     /Count 0\n"
		"  >>\n"
		"endobj\n\n");

	// Next is the "Pages" object (this is object 3), which references the
	// individual pages.  The page objects are indexed 4, 5, ...
	// (The "/Kids" field is an array of page objects, I think)
	int obj = 3;
	objects[obj] = new char[256 + 16 * n_pages];
	sprintf(objects[obj], "3 0 obj\n"
		"  << /Type /Pages\n"
		"     /Kids [ ");
	for (int k = 0; k < n_pages; k++)
		sprintf(objects[obj] + strlen(objects[obj]), "%d 0 R ", k + 4);
	sprintf(objects[obj] + strlen(objects[obj]), "]\n");
	sprintf(objects[obj] + strlen(objects[obj]),
		"     /Count %d\n"
		"  >>\n"
		"endobj\n\n", n_pages);

	// Set some values
	int first_page = 4;
	int first_font = first_page + n_pages;
	int first_page_content = first_font + font_count;

	// Now come the page objects
	obj = 4;
	for (int k = 0; k < n_pages; k++) {
		objects[obj] = new char[1024 + 64 * font_count];
		sprintf(objects[obj], "%d 0 obj\n"
			"  << /Type /Page\n"
			"     /Parent 3 0 R\n"
			"     /MediaBox [ 0 0 %d %d ]\n"
			"     /Contents %d 0 R\n",
			obj,
			(int)width, (int)height,
			first_page_content + 2 * k);
		// add the "Resources"
		// the first is the /ProcSet
		sprintf(objects[obj] + strlen(objects[obj]),
			"     /Resources << /ProcSet %d 0 R\n",
			first_page_content + 2 * k + 1);
		// add a font to the /Font dictionary for each of the document fonts
		sprintf(objects[obj] + strlen(objects[obj]),
			"                   /Font << \n");
		// (this is probably wasteful, because not every page uses
		// all the fonts, but oh well)
		int font_index = first_font;
		for (int k = 0; k < max_fonts; k++) {
			if (fonts[k]) {
				sprintf(objects[obj] + strlen(objects[obj]),
					"                        /F%d %d 0 R\n",
					k, font_index);
				font_index++;
			}
		}

		// finish the page object
		sprintf(objects[obj] + strlen(objects[obj]),
			"                        >>\n"
			"                >>\n"
			"  >>\n"
			"endobj\n\n");
		obj++;
	}

	// Add font object (a font dictionary) for each of the document fonts
	// (a this point 'obj' equals 'first_font')
	for (int k = 0; k < max_fonts; k++) {
		if (fonts[k]) {
			objects[obj] = new char[256];
			sprintf(objects[obj], "%d 0 obj\n"
				"  << /Type /Font\n"
				"     /Subtype /Type1\n"
				"     /Name /F%d\n"
				"     /BaseFont /%s\n"
				"     /Encoding /MacRomanEncoding\n"
				"  >>\n"
				"endobj\n\n",
				obj, k, FontNames[k]);
			obj++;
		}
	}


	// For each page object there are two other objects:
	// the "stream" object containing the PDF commands, then
	// a "procset" object, which I don't really understand,
	for (int k = 0; k < n_pages; k++) {
		// The stream object comes first
		char *stream = pages[k].stream.text;
		objects[obj] = new char[256 + strlen(stream)];
		sprintf(objects[obj], "%d 0 obj\n"
			"  << /Length %d >>\n"
			"stream\n"
			"%s\n"
			"endstream\n"
			"endobj\n\n",
			obj, (int)(strlen(stream) + 1), stream);
		obj++;

		// next is the procset object
		objects[obj] = new char[256];
		sprintf(objects[obj], "%d 0 obj\n  [/PDF /Text]\nendobj\n\n",
			obj);
		obj++;
	}

	if (obj >= max_objects) {
		fprintf(stderr, "Too many objects!\n");
		exit(1);
	}

	// Now write all the objects
	int size = 0;
	for (int k = 0; k < obj; k++) {
		fprintf(out, "%s", objects[k]);
		size += strlen(objects[k]);
	}
	int start_xref = size;

	// Write the "xref" section
	fprintf(out, "xref\n0 %d\n", obj);
	size = 0;
	fprintf(out, "0000000000 65536 f \n");
	for (int k = 1; k < obj; k++) {
		size += strlen(objects[k - 1]);
		fprintf(out, "%010d %05d n \n", size, 0);
	}

	// Write the trailer
	fprintf(out, "\ntrailer\n"
		"  << /Size %d\n"
		"     /Root 1 0 R\n"
		"  >>\n"
		"startxref\n"
		"%d\n"
		"%%%%EOF\n", obj, start_xref);

	fclose(out);

	// free all the "objects"
	for (int k = 0; k < obj; k++)
		delete[] objects[k];
	delete[] objects;
}


/************/
/* Commands */
/************/

/*
int main( int argc, char *argv[] )
{
  PDF pdf("out.pdf");
  pdf.new_page();
  //pdf.arc(320, 320,  180, 0, -M_PI/2);
  //pdf.arcn(320, 320,  180, 0, M_PI/2);
  //pdf.stroke();

  pdf.selectfont(Helvetica, 36);
  //pdf.position_text("hello, world!", 320, 320, 0.5, 0.5);
  //pdf.position_text("hello, world!\nhow are you?\nwe're fine",
  //		    320, 320, 0.5, 0.5);

  pdf.setgray_nonstroke(0.75);
  pdf.text_box("hello world!", 320, 320, 12, 18);

  pdf.finish();
}
*/

