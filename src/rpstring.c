/* Taken from Allegro 4.4.  The routines were by Michael Bukin. */


#include <ctype.h>
#include "a4aux.h"



/* rp_strlwr:
 *  Convert all upper case characters in string to lower case.
 */
char *rp_strlwr(char *string)
{
   char *p;
   ASSERT(string);

   for (p=string; *p; p++)
      *p = utolower(*p);

   return string;
}



/* rp_strupr:
 *  Convert all lower case characters in string to upper case.
 */
char *rp_strupr(char *string)
{
   char *p;
   ASSERT(string);

   for (p=string; *p; p++)
      *p = utoupper(*p);

   return string;
}



/* rp_stricmp:
 *  Case-insensitive comparison of 7-bit ASCII strings.
 */
int rp_stricmp(const char *s1, const char *s2)
{
   int c1, c2;
   ASSERT(s1);
   ASSERT(s2);

   do {
      c1 = tolower(*(s1++));
      c2 = tolower(*(s2++));
   } while ((c1) && (c1 == c2));

   return c1 - c2;
}
