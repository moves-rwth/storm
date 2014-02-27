/* glpsdf.h */

/***********************************************************************
*  This code is part of GLPK (GNU Linear Programming Kit).
*
*  Copyright (C) 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008,
*  2009, 2010, 2011, 2013 Andrew Makhorin, Department for Applied
*  Informatics, Moscow Aviation Institute, Moscow, Russia. All rights
*  reserved. E-mail: <mao@gnu.org>.
*
*  GLPK is free software: you can redistribute it and/or modify it
*  under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  GLPK is distributed in the hope that it will be useful, but WITHOUT
*  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
*  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public
*  License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with GLPK. If not, see <http://www.gnu.org/licenses/>.
***********************************************************************/

#ifndef GLPSDF_H
#define GLPSDF_H

typedef struct glp_data glp_data;
/* plain data file */

glp_data *glp_sdf_open_file(const char *fname);
/* open plain data file */

void glp_sdf_set_jump(glp_data *data, void *jump);
/* set up error handling */

void glp_sdf_error(glp_data *data, const char *fmt, ...);
/* print error message */

void glp_sdf_warning(glp_data *data, const char *fmt, ...);
/* print warning message */

int glp_sdf_read_int(glp_data *data);
/* read integer number */

double glp_sdf_read_num(glp_data *data);
/* read floating-point number */

const char *glp_sdf_read_item(glp_data *data);
/* read data item */

const char *glp_sdf_read_text(glp_data *data);
/* read text until end of line */

int glp_sdf_line(glp_data *data);
/* determine current line number */

void glp_sdf_close_file(glp_data *data);
/* close plain data file */

#endif

/* eof */
