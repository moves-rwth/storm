dnl Copyright (C) 2004-2017 Julien Pommier
dnl 
dnl This file is  free software;  you  can  redistribute  it  and/or modify it
dnl under  the  terms  of the  GNU  Lesser General Public License as published
dnl by  the  Free Software Foundation;  either version 3 of the License,  or
dnl (at your option) any later version along with the GCC Runtime Library
dnl Exception either version 3.1 or (at your option) any later version.
dnl This program  is  distributed  in  the  hope  that it will be useful,  but
dnl WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
dnl or  FITNESS  FOR  A PARTICULAR PURPOSE.  See the GNU Lesser General Public
dnl License and GCC Runtime Library Exception for more details.
dnl You  should  have received a copy of the GNU Lesser General Public License
dnl along  with  this program;  if not, write to the Free Software Foundation,
dnl Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301, USA.

AC_DEFUN([AC_CHECK_CXX_FLAG],
[AC_MSG_CHECKING([whether ${CXX} accepts $1])

echo 'int main(){}' > conftest.c
if test -z "`${CXX} $1 -o conftest conftest.c 2>&1`"; then
  $2="${$2} $1"
  echo "yes"
else
  echo "no"
  $3
fi
dnl echo "$2=${$2}"
rm -f conftest*
])

