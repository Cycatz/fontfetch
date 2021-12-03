#include <ft2build.h>
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include FT_FREETYPE_H

#define COLOR_BOLD "\x1b[1m"
#define COLOR_RED "\x1b[1;31m"
#define COLOR_GREEN "\x1b[1;32m"
#define COLOR_YELLOW "\x1b[1;33m"
#define COLOR_BLUE "\x1b[1;34m"
#define COLOR_MAGENTA "\x1b[1;35m"
#define COLOR_CYAN "\x1b[1;36m"
#define COLOR_RESET "\x1b[0m"


#define VALUE_TYPE_STR (1 << 0)
#define VALUE_TYPE_YESNO (1 << 1)
#define VALUE_TYPE_AVAIL (1 << 2)
#define VALUE_TYPE_INT (1 << 3)
#define VALUE_TYPE_LONGINT (1 << 4)


static FT_Library library;
static FT_Face face;


void print_line(const char *field, const void *value, int value_type)
{
#define YESNO(x) \
    (x) ? COLOR_GREEN "yes" COLOR_RESET : COLOR_RED "no" COLOR_RESET

#define AVAIL(x)                              \
    (x) ? COLOR_GREEN "available" COLOR_RESET \
        : COLOR_RED "unavailable" COLOR_RESET

    /* 1. print the field */
    printf("* %-20s: ", field);


    /* 2. print the value */
    switch (value_type) {
    case VALUE_TYPE_STR:
        printf(COLOR_BOLD "%s\n" COLOR_RESET, (const char *) value);
        break;
    case VALUE_TYPE_INT:
        printf(COLOR_BOLD "%d\n" COLOR_RESET, (int) value);
        break;
    case VALUE_TYPE_LONGINT:
        printf(COLOR_BOLD "%ld\n" COLOR_RESET, (long int) value);
        break;
    case VALUE_TYPE_AVAIL:
        printf("%s\n", AVAIL(value));
        break;
    case VALUE_TYPE_YESNO:
        printf("%s\n", YESNO(value));
        break;
    }
}

void print_font_info()
{
    printf("==================== Font ====================\n");

    /* Access FT_FaceRec */
    print_line("Width", face->family_name, VALUE_TYPE_STR);
    print_line("Height", face->style_name, VALUE_TYPE_STR);
    print_line("Number of glyphs", (const void *) face->num_glyphs,
               VALUE_TYPE_LONGINT);

    /* Pass integer as pointer */
    print_line("Outline glyphs",
               (const void *) (face->face_flags & FT_FACE_FLAG_SCALABLE),
               VALUE_TYPE_AVAIL);
    print_line("Bitmap glyphs",
               (const void *) (face->face_flags & FT_FACE_FLAG_FIXED_SIZES),
               VALUE_TYPE_AVAIL);
    print_line("Fixed width",
               (const void *) (face->face_flags & FT_FACE_FLAG_FIXED_WIDTH),
               VALUE_TYPE_YESNO);
    print_line("Horizontal metrics",
               (const void *) (face->face_flags & FT_FACE_FLAG_HORIZONTAL),
               VALUE_TYPE_AVAIL);
    print_line("Vertical metrics",
               (const void *) (face->face_flags & FT_FACE_FLAG_VERTICAL),
               VALUE_TYPE_AVAIL);
    print_line("Kerning info",
               (const void *) (face->face_flags & FT_FACE_FLAG_KERNING),
               VALUE_TYPE_AVAIL);
    print_line("Color", (const void *) (face->face_flags & FT_FACE_FLAG_COLOR),
               VALUE_TYPE_AVAIL);
}

void print_glyph_info(uint32_t charcode, FT_GlyphSlot glyph)
{
    printf("=================== Glyph ===================\n");

    print_line("Char code", (const void *) charcode, VALUE_TYPE_INT);
    print_line("Glyph index", (const void *) glyph->glyph_index,
               VALUE_TYPE_INT);
    print_line("Width", (const void *) glyph->metrics.width,
               VALUE_TYPE_LONGINT);
    print_line("Height", (const void *) glyph->metrics.height,
               VALUE_TYPE_LONGINT);
    print_line("HoriBearingX", (const void *) glyph->metrics.horiBearingX,
               VALUE_TYPE_LONGINT);
    print_line("HoriBearingY", (const void *) glyph->metrics.horiBearingY,
               VALUE_TYPE_LONGINT);
    print_line("HoriAdvance", (const void *) glyph->metrics.horiAdvance,
               VALUE_TYPE_LONGINT);
    print_line("VertBearingX", (const void *) glyph->metrics.vertBearingX,
               VALUE_TYPE_LONGINT);
    print_line("VertBearingY", (const void *) glyph->metrics.vertBearingY,
               VALUE_TYPE_LONGINT);
    print_line("VertAdvance", (const void *) glyph->metrics.vertAdvance,
               VALUE_TYPE_LONGINT);
}

void read_font(const char *filename)
{
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

    print_font_info();
}

void read_glyph(uint32_t charcode)
{
    FT_Error error;
    FT_UInt glyph_index;

    glyph_index = FT_Get_Char_Index(face, charcode);
    error = FT_Load_Glyph(face, glyph_index, FT_LOAD_NO_SCALE);

    if (error) {
        fprintf(stderr, "Unknown error\n");
        exit(EXIT_FAILURE);
    }

    print_glyph_info(charcode, face->glyph);
}


void usage(const char *prog_name)
{
    fprintf(stderr, "Usage: %s -f <file> [-c char]\n", prog_name);
}


int main(int argc, char *argv[])
{
    int opt;
    char *file_path = NULL;
    char *glyph_char = NULL;

    while ((opt = getopt(argc, argv, "f:c:h")) != -1) {
        switch (opt) {
        case 'f':
            file_path = optarg;
            break;
        case 'c':
            glyph_char = optarg;
            break;
        case 'h':
            usage(*argv);
            exit(EXIT_SUCCESS);
        case '?':
            fprintf(stderr, "Unknown option: %c\n", optopt);
            usage(*argv);
            exit(EXIT_FAILURE);
        case ':':
            fprintf(stderr, "Missing arg for %c\n", optopt);
            usage(*argv);
            exit(EXIT_FAILURE);
        }
    }

    if (!file_path) {
        usage(*argv);
        exit(EXIT_FAILURE);
    }


    if (file_path)
        read_font(file_path);
    if (glyph_char)
        read_glyph(glyph_char[0]);


    return 0;
}
