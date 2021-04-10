#include "print.h"

#include <zlib.h>
#include <SDL2/SDL.h>


static const char *basename(const char *exec) {
  
  const char *name = strrchr(exec, '/');
  if (name == NULL) {
    return exec;
  }
  return name + 1;
}


void print_version(const char *exec) {
  printf("version: 0.0.1 (DEV)\n");
  printf("  using: zlib %s\n", zlibVersion());
  
  SDL_version compiled;
  SDL_VERSION(&compiled);
  printf("         sdl2 %d.%d.%d\n", compiled.major, compiled.minor, compiled.patch);
}


void print_help(const char *exec) {
  const char *name = basename(exec);
  
  printf("usage: %s [option] [file]\n", name);
  printf("option:\n");
  printf("        --version              Print version\n");
  printf("        --help                 List available commandes\n");
  printf("        --chunk                Print all chunks in the file\n");
  printf("        --display              Display the file\n");
  printf("        --bmp=<filename>       Save file into a BMP file\n");
  printf("\n");

  printf("source: https://github.com/gloutch/png-plte\n");
}


static void print_IHDR(const struct IHDR *chunk) {

  printf("[%d,%d]   ", chunk->width, chunk->height);
  printf("depth %d   ", chunk->depth);

  switch(chunk->color_type) {
  case GRAYSCALE:
    printf("gray");
    break;
  case RGB_TRIPLE:
    printf("RGB");
    break;
  case PLTE_INDEX:
    printf("RGB palette");
    break;
  case GRAYSCALE_ALPHA:
    printf("gray+alpha");
    break;
  case RGB_TRIPLE_ALPHA:
    printf("RGB+alpha");
    break;
  default:
    printf("%d:UNKNOWN (color-type)", chunk->color_type);
  }
  printf("   ");

  if (chunk->compression == 0) {
    printf("deflate/inflate");
  } else {
    printf("%d:UNKNOWN (compression)", chunk->compression);
  }
  printf("   ");

  if (chunk->filter == 0) {
    printf("adaptive");
  } else {
    printf("%d:UNKNOWN (filter)", chunk->filter);
  }
  printf("   ");

  switch(chunk->interlace) {
  case 0:
    printf("no-interlace");
    break;
  case 1:
    printf("adam7");
    break;
  default:
    printf("%d:UNKNOWN (interlace)", chunk->interlace);
  }
}

static void print_PLTE(const struct PLTE *chunk) {
  printf("%d colors", chunk->nb_color);
}

static void print_BKGD(const struct BKGD *chunk) {
  switch (chunk->color_type) {
  case PLTE_INDEX: {
    printf("index %d", chunk->color.index);
    return;
  }
  case GRAYSCALE:
  case GRAYSCALE_ALPHA: {
    printf("gray: %d", chunk->color.gray);
    return;
  }
  case RGB_TRIPLE:
  case RGB_TRIPLE_ALPHA: {
    printf("red %d   green %d   blue %d", chunk->color.rgb.red, chunk->color.rgb.green, chunk->color.rgb.blue);
  }
  }
}

static void print_PHYS(const struct PHYS *chunk) {
  printf("X:%d  Y:%d  unit:%d ", chunk->x_axis, chunk->y_axis, chunk->unit);
  switch (chunk->unit) {
  case 0:
    printf("(ratio)");
    break;
  case 1:
    printf("(meter)");
    break;
  default:
    printf("(UNKNOWN)");
  }
}

static void print_TIME(const struct TIME *chunk) {
  printf("%d/%02d/%02d ", chunk->day, chunk->month, chunk->year);
  printf("%02d:%02d:%02d", chunk->hour, chunk->minute, chunk->second);
}



void print_chunk(const struct chunk *chunk, const struct IHDR *header) {

  uint32_t chunk_size = chunk->length + 12;

  if (chunk->type == UKWN) {
    printf("ukwn %-6d  ", chunk_size);
  }
  else {
    uint32_t type_value = enum_to_type_value(chunk->type);
    printf("%.4s %-6d  ", (char *) &(type_value), chunk_size);

    switch (chunk->type) {
    case IHDR: {
      const struct IHDR t = IHDR_chunk(chunk);
      print_IHDR(&t);
      break;
    }
    case PLTE: {
      const struct PLTE t = PLTE_chunk(chunk, header);
      print_PLTE(&t);
      break;
    }
    case GAMA: {
      uint32_t gamma = GAMA_chunk(chunk);
      printf("gamma %d/100000", gamma);
      break;
    }
    case BKGD: {
      const struct BKGD t = BKGD_chunk(chunk, header);
      print_BKGD(&t);
      break;
    }
    case PHYS: {
      const struct PHYS t = PHYS_chunk(chunk);
      print_PHYS(&t);
      break;
    }
    case TIME: {
      const struct TIME t = TIME_chunk(chunk);
      print_TIME(&t);
      break;
    }
    default:; // nothing for now
    }
  }

  uint32_t computed_crc = crc((unsigned char *) (chunk->data - 4), 4 + chunk->length);
  if (chunk->crc != computed_crc) {
    printf(" [CRC 0x%x mismatch (0x%x computed)]\n", chunk->crc, computed_crc);
  } else {
    printf("\n");
  }
}



void print_PNG_file(const struct mfile *file) {
  // print file info
  printf("%s  %6zu Byte\n\n", file->pathname, file->size);

  size_t remainder = file->size;
  uint8_t *cursor = file->data;

  // print the 8-byte signature
  printf("SIG  8       ");
  for (int i = 0; i < 8; i++) {
    printf("%02X ", cursor[i]);
  }
  printf("\n");

  // print every chunk
  remainder -= 8;
  cursor = file->data + (file->size - remainder);
  struct chunk current = get_chunk(remainder, cursor);

  assert(current.type == IHDR); // first chunk is the header
  const struct IHDR header = IHDR_chunk(&current);
  
  while (current.type != IEND) {
    print_chunk(&current, &header);
    
    remainder -= current.length + 12; // size of the chunk
    cursor = file->data + (file->size - remainder);
    current = get_chunk(remainder, cursor);
  }
  print_chunk(&current, NULL);
}
