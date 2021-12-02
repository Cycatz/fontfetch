#include <stdio.h>
#include <ft2build.h>
#include FT_FREETYPE_H  

#define COLOR_BOLD    "\x1b[1m"
#define COLOR_RED     "\x1b[1;31m"
#define COLOR_GREEN   "\x1b[1;32m"
#define COLOR_YELLOW  "\x1b[1;33m"
#define COLOR_BLUE    "\x1b[1;34m"
#define COLOR_MAGENTA "\x1b[1;35m"
#define COLOR_CYAN    "\x1b[1;36m"
#define COLOR_RESET   "\x1b[0m"


#define VALUE_TYPE_STR     (1 << 0)
#define VALUE_TYPE_YESNO   (1 << 1)
#define VALUE_TYPE_AVAIL   (1 << 2)
#define VALUE_TYPE_INT     (1 << 3)
#define VALUE_TYPE_LONGINT (1 << 4)


void print_line(const char *field, const void *value, int value_type)
{
    #define YESNO(x) (x) ? COLOR_GREEN "yes" COLOR_RESET : \
                           COLOR_RED   "no"  COLOR_RESET

    #define AVAIL(x) (x) ? COLOR_GREEN "available"   COLOR_RESET : \
                           COLOR_RED   "unavailable" COLOR_RESET

    /* 1. print the field */  
    printf("* %-20s: ", field);


    /* 2. print the value */
    switch (value_type) {
    case VALUE_TYPE_STR:
        printf(COLOR_BOLD "%s\n" COLOR_RESET, (const char *)value); 
        break;
    case VALUE_TYPE_LONGINT:
        printf(COLOR_BOLD "%ld\n" COLOR_RESET, *(const long int *) value);
        break;
    case VALUE_TYPE_AVAIL:
        printf("%s\n", AVAIL(value));
        break;
    case VALUE_TYPE_YESNO:
        printf("%s\n", YESNO(value));
        break;
    }
}
void print_font_info(FT_Face face)
{
    printf("==================== Font ====================\n");

    /* Access FT_FaceRec */
    print_line("Number of glyphs", &face->num_glyphs, VALUE_TYPE_LONGINT);
    print_line("Family name",      face->family_name, VALUE_TYPE_STR);   
    print_line("Style name",       face->style_name,  VALUE_TYPE_STR);

    /* Pass integer as pointer */
    print_line("Outline glyphs",
               (const void *) (face->face_flags & FT_FACE_FLAG_SCALABLE),    VALUE_TYPE_AVAIL);
    print_line("Bitmap glyphs",
               (const void *) (face->face_flags & FT_FACE_FLAG_FIXED_SIZES), VALUE_TYPE_AVAIL);
    print_line("Fixed width",
               (const void *) (face->face_flags & FT_FACE_FLAG_FIXED_WIDTH), VALUE_TYPE_YESNO);
    print_line("Horizontal metrics",
               (const void *) (face->face_flags & FT_FACE_FLAG_HORIZONTAL),  VALUE_TYPE_AVAIL);
    print_line("Vertical metrics",
               (const void *) (face->face_flags & FT_FACE_FLAG_VERTICAL),    VALUE_TYPE_AVAIL);
    print_line("Kerning info",
               (const void *) (face->face_flags & FT_FACE_FLAG_KERNING),     VALUE_TYPE_AVAIL);
    print_line("Color",
               (const void *) (face->face_flags & FT_FACE_FLAG_COLOR),       VALUE_TYPE_AVAIL);
}

void read_font(const char *filename)
{
    FT_Library library;
    FT_Face face;
    FT_Error error; 


    error = FT_Init_FreeType(&library);
    if (error) {
        fprintf(stderr, "Error when initializing the font library!\n");
        exit(EXIT_FAILURE);
    } 

    error = FT_New_Face(library, filename, 0, &face);
    if (error == FT_Err_Unknown_File_Format) {
        fprintf(stderr, "Error!! Unknown file format!\n"); 
        exit(EXIT_FAILURE);
    } else if (error) {
        fprintf(stderr, "Unknown error!\n"); 
        exit(EXIT_FAILURE);
    }

    print_font_info(face);
}


int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <font file>\n", argv[0]); 
        exit(EXIT_FAILURE);
    }
    read_font(argv[1]);
}
