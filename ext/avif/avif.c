#include "ruby.h"
#include "ruby/thread.h"
#include "avif/avif.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s) + 1;
    char *p = malloc(len);
    if (p) {
        memcpy(p, s, len);
    }
    return p;
}

typedef struct {
    char *path;
    uint8_t *pixels;
    uint32_t width;
    uint32_t height;
    avifResult result;
    char *error_message;
} decode_payload_t;

static void *decode(void *arg) {
    decode_payload_t *payload = (decode_payload_t *)arg;

    payload->pixels = NULL;
    payload->result = AVIF_RESULT_UNKNOWN_ERROR;
    payload->error_message = NULL;

    avifDecoder *decoder = avifDecoderCreate();
    if (!decoder) {
        payload->error_message = safe_strdup("avifDecoderCreate failed (out of memory)");
        free(payload->path);
        return NULL;
    }

    payload->result = avifDecoderSetIOFile(decoder, payload->path);
    free(payload->path);
    payload->path = NULL;
    if (payload->result != AVIF_RESULT_OK) {
        avifDecoderDestroy(decoder);
        return NULL;
    }

    payload->result = avifDecoderParse(decoder);
    if (payload->result != AVIF_RESULT_OK) {
        avifDecoderDestroy(decoder);
        return NULL;
    }

    payload->result = avifDecoderNextImage(decoder);
    if (payload->result != AVIF_RESULT_OK) {
        avifDecoderDestroy(decoder);
        return NULL;
    }

    avifImage *image = decoder->image;
    payload->width = image->width;
    payload->height = image->height;

    avifRGBImage rgb;
    avifRGBImageSetDefaults(&rgb, image);
    rgb.format = AVIF_RGB_FORMAT_RGBA;
    rgb.depth = 8;
    
    size_t pixel_size = (size_t)rgb.width * rgb.height * avifRGBImagePixelSize(&rgb);
    payload->pixels = malloc(pixel_size);
    if (!payload->pixels) {
        payload->error_message = safe_strdup("Failed to allocate pixel buffer (out of memory)");
        avifDecoderDestroy(decoder);
        return NULL;
    }
    rgb.pixels = payload->pixels;
    rgb.rowBytes = rgb.width * avifRGBImagePixelSize(&rgb);

    payload->result = avifImageYUVToRGB(image, &rgb);
    if (payload->result != AVIF_RESULT_OK) {
        free(payload->pixels);
        payload->pixels = NULL;
    }
    
    avifDecoderDestroy(decoder);
    return NULL;
}

static VALUE avif_decode(VALUE self, VALUE path) {
    Check_Type(path, T_STRING);

    decode_payload_t payload;
    
    long path_len = RSTRING_LEN(path);
    payload.path = malloc(path_len + 1);
    if (!payload.path) {
        rb_raise(rb_eNoMemError, "Failed to allocate memory for path string");
    }
    memcpy(payload.path, RSTRING_PTR(path), path_len);
    payload.path[path_len] = '\0';

    rb_thread_call_without_gvl(decode, &payload, RUBY_UBF_IO, NULL);

    if (payload.error_message) {
        char *err_msg = payload.error_message;
        rb_raise(rb_eRuntimeError, "AVIF decoding failed: %s", err_msg);
        free(err_msg);
    }
    if (payload.result != AVIF_RESULT_OK) {
        rb_raise(rb_eRuntimeError, "AVIF decoding failed: %s", avifResultToString(payload.result));
    }

    size_t pixel_bytes = (size_t)payload.width * payload.height * 4;
    
    VALUE pixel_string = rb_str_new((const char*)payload.pixels, pixel_bytes);
    free(payload.pixels);

    rb_str_freeze(pixel_string);

    VALUE result_array = rb_ary_new_capa(3);
    rb_ary_push(result_array, UINT2NUM(payload.width));
    rb_ary_push(result_array, UINT2NUM(payload.height));
    rb_ary_push(result_array, pixel_string);
    
    rb_ary_freeze(result_array);

    return result_array;
}

void Init_avif(void) {
    rb_ext_ractor_safe(true);
    
    VALUE rb_mAvif = rb_define_module("Avif");
    rb_define_singleton_method(rb_mAvif, "decode", avif_decode, 1);
}