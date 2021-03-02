#include "print.h"



void print_version(void) {
  printf("Version (DEV) 0.0.1\n");
  printf("         zlib %s\n", zlibVersion());
  printf("\n");
}



static void print_IHDR_chunk(const struct IHDR *chunk) {

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
    printf("RGB+palette");
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



static void print_TIME_chunk(const struct TIME *chk) {
  printf("%d/%02d/%02d ", chk->day, chk->month, chk->year);
  printf("%02d:%02d:%02d", chk->hour, chk->minute, chk->second);
}



void print_chunk(const struct chunk *chunk) {

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
      print_IHDR_chunk(&t);
      break;
    }
    case GAMA: {
      uint32_t gamma = GAMA_chunk(chunk);
      printf("gamma %d/100000", gamma);
      break;
    }
    case TIME: {
      const struct TIME t = TIME_chunk(chunk);
      print_TIME_chunk(&t);
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

  while (current.type != IEND) {
    print_chunk(&current);
    
    remainder -= current.length + 12; // size of the chunk
    cursor = file->data + (file->size - remainder);
    current = get_chunk(remainder, cursor);
  }
  print_chunk(&current);
}
