// TODO type -> u8
#include <node_api.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <jpeglib.h>
#include "type.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
//    decode
////////////////////////////////////////////////////////////////////////////////////////////////////
struct my_jpeg_error_mgr {
  struct jpeg_error_mgr pub;
  jmp_buf setjmp_buffer;
};

void jpeg_cb_error_exit(j_common_ptr cinfo){
  struct my_jpeg_error_mgr *mptr;
  mptr=(struct my_jpeg_error_mgr*) cinfo->err;
  //(*cinfo->err->output_message) (cinfo);
  longjmp(mptr->setjmp_buffer, 1);
}

napi_value jpeg_decode_rgba(napi_env env, napi_callback_info info) {
  napi_status status;
  
  napi_value ret_dummy;
  status = napi_create_int32(env, 0, &ret_dummy);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value ret_dummy");
    return ret_dummy;
  }
  
  size_t argc = 2;
  napi_value argv[2];
  status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Failed to parse arguments");
    return ret_dummy;
  }
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  u8 *data_src;
  size_t data_src_len;
  status = napi_get_buffer_info(env, argv[0], (void**)&data_src, &data_src_len);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Invalid buffer was passed as argument of data_src");
    return ret_dummy;
  }
  
  u8 *data_dst;
  size_t data_dst_len;
  status = napi_get_buffer_info(env, argv[1], (void**)&data_dst, &data_dst_len);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Invalid buffer was passed as argument of data_dst");
    return ret_dummy;
  }
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  struct jpeg_decompress_struct cinfo;
  struct my_jpeg_error_mgr jerr;
  
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit=jpeg_cb_error_exit;
  
  if (setjmp(jerr.setjmp_buffer)) {
    jpeg_destroy_decompress(&cinfo);
    napi_throw_error(env, NULL, "fail setjmp jerr.setjmp_buffer");
    return ret_dummy;
  }
  
  jpeg_create_decompress(&cinfo);
  jpeg_mem_src(&cinfo, (u8 *) data_src, data_src_len);
  jpeg_read_header(&cinfo, TRUE);
  cinfo.out_color_space = JCS_EXT_RGBA;
  jpeg_start_decompress(&cinfo);
  
  u32 width, height, line;
  
  width = cinfo.output_width;
  height = cinfo.output_height;
  
  if (data_dst_len < 4*width*height) {
    jpeg_destroy_decompress(&cinfo);
    printf("width = %d\n", width);
    printf("height = %d\n", height);
    printf("required = %d\n", 4*width*height);
    printf("data_dst_len = %ld\n", data_dst_len);
    napi_throw_error(env, NULL, "data_dst is too small");
    return ret_dummy;
  }
  
  while((line = cinfo.output_scanline) < height){
    u8 *ptr = data_dst + 4*line*width;
    jpeg_read_scanlines(&cinfo, &ptr, 1);
  }
  
  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  
  napi_value ret_arr;
  status = napi_create_array(env, &ret_arr);
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value");
    return ret_dummy;
  }
  
  
  // width
  napi_value ret_width;
  status = napi_create_int32(env, width, &ret_width);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value ret_width");
    return ret_dummy;
  }
  
  status = napi_set_element(env, ret_arr, 0, ret_width);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value ret_width assign");
    return ret_dummy;
  }
  // height
  napi_value ret_height;
  status = napi_create_int32(env, height, &ret_height);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value ret_height");
    return ret_dummy;
  }
  
  status = napi_set_element(env, ret_arr, 1, ret_height);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value ret_height assign");
    return ret_dummy;
  }
  
  // byte size = 4
  napi_value ret_byte_size;
  status = napi_create_int32(env, 4, &ret_byte_size);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value ret_byte_size");
    return ret_dummy;
  }
  
  status = napi_set_element(env, ret_arr, 2, ret_byte_size);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value ret_byte_size assign");
    return ret_dummy;
  }
  
  
  return ret_arr;
}

napi_value jpeg_decode_rgb(napi_env env, napi_callback_info info) {
  napi_status status;
  
  napi_value ret_dummy;
  status = napi_create_int32(env, 0, &ret_dummy);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value ret_dummy");
    return ret_dummy;
  }
  
  size_t argc = 2;
  napi_value argv[2];
  status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Failed to parse arguments");
    return ret_dummy;
  }
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  u8 *data_src;
  size_t data_src_len;
  status = napi_get_buffer_info(env, argv[0], (void**)&data_src, &data_src_len);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Invalid buffer was passed as argument of data_src");
    return ret_dummy;
  }
  
  u8 *data_dst;
  size_t data_dst_len;
  status = napi_get_buffer_info(env, argv[1], (void**)&data_dst, &data_dst_len);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Invalid buffer was passed as argument of data_dst");
    return ret_dummy;
  }
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  struct jpeg_decompress_struct cinfo;
  struct my_jpeg_error_mgr jerr;
  
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit=jpeg_cb_error_exit;
  
  if (setjmp(jerr.setjmp_buffer)) {
    jpeg_destroy_decompress(&cinfo);
    napi_throw_error(env, NULL, "fail setjmp jerr.setjmp_buffer");
    return ret_dummy;
  }
  
  jpeg_create_decompress(&cinfo);
  jpeg_mem_src(&cinfo, (u8 *) data_src, data_src_len);
  jpeg_read_header(&cinfo, TRUE);
  cinfo.out_color_space = JCS_EXT_RGB;
  jpeg_start_decompress(&cinfo);
  
  u32 width, height, line;
  
  width = cinfo.output_width;
  height = cinfo.output_height;
  
  if (data_dst_len < 3*width*height) {
    jpeg_destroy_decompress(&cinfo);
    printf("width = %d\n", width);
    printf("height = %d\n", height);
    printf("required = %d\n", 3*width*height);
    printf("data_dst_len = %ld\n", data_dst_len);
    napi_throw_error(env, NULL, "data_dst is too small");
    return ret_dummy;
  }
  
  while((line = cinfo.output_scanline) < height){
    u8 *ptr = data_dst + 3*line*width;
    jpeg_read_scanlines(&cinfo, &ptr, 1);
  }
  
  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  
  napi_value ret_arr;
  status = napi_create_array(env, &ret_arr);
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value");
    return ret_dummy;
  }
  
  
  // width
  napi_value ret_width;
  status = napi_create_int32(env, width, &ret_width);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value ret_width");
    return ret_dummy;
  }
  
  status = napi_set_element(env, ret_arr, 0, ret_width);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value ret_width assign");
    return ret_dummy;
  }
  // height
  napi_value ret_height;
  status = napi_create_int32(env, height, &ret_height);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value ret_height");
    return ret_dummy;
  }
  
  status = napi_set_element(env, ret_arr, 1, ret_height);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value ret_height assign");
    return ret_dummy;
  }
  
  // byte size = 4
  napi_value ret_byte_size;
  status = napi_create_int32(env, 4, &ret_byte_size);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value ret_byte_size");
    return ret_dummy;
  }
  
  status = napi_set_element(env, ret_arr, 2, ret_byte_size);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value ret_byte_size assign");
    return ret_dummy;
  }
  
  
  return ret_arr;
}

napi_value jpeg_decode_size(napi_env env, napi_callback_info info) {
  napi_status status;
  
  napi_value ret_dummy;
  status = napi_create_int32(env, 0, &ret_dummy);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value ret_dummy");
    return ret_dummy;
  }
  
  size_t argc = 1;
  napi_value argv[1];
  status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Failed to parse arguments");
    return ret_dummy;
  }
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  u8 *data_src;
  size_t data_src_len;
  status = napi_get_buffer_info(env, argv[0], (void**)&data_src, &data_src_len);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Invalid buffer was passed as argument of data_src");
    return ret_dummy;
  }
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  struct jpeg_decompress_struct cinfo;
  struct my_jpeg_error_mgr jerr;
  
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit=jpeg_cb_error_exit;
  
  if (setjmp(jerr.setjmp_buffer)) {
    jpeg_destroy_decompress(&cinfo);
    napi_throw_error(env, NULL, "fail setjmp jerr.setjmp_buffer");
    return ret_dummy;
  }
  
  jpeg_create_decompress(&cinfo);
  jpeg_mem_src(&cinfo, (u8 *) data_src, data_src_len);
  jpeg_read_header(&cinfo, TRUE);
  cinfo.out_color_space = JCS_EXT_RGBA;
  jpeg_start_decompress(&cinfo);
  
  u32 width, height;
  
  width = cinfo.output_width;
  height = cinfo.output_height;
  
  jpeg_destroy_decompress(&cinfo);
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  
  napi_value ret_arr;
  status = napi_create_array(env, &ret_arr);
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value");
    return ret_dummy;
  }
  
  
  // width
  napi_value ret_width;
  status = napi_create_int32(env, width, &ret_width);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value ret_width");
    return ret_dummy;
  }
  
  status = napi_set_element(env, ret_arr, 0, ret_width);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value ret_width assign");
    return ret_dummy;
  }
  // height
  napi_value ret_height;
  status = napi_create_int32(env, height, &ret_height);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value ret_height");
    return ret_dummy;
  }
  
  status = napi_set_element(env, ret_arr, 1, ret_height);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value ret_height assign");
    return ret_dummy;
  }
  
  // byte size = 4
  napi_value ret_byte_size;
  status = napi_create_int32(env, 4, &ret_byte_size);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value ret_byte_size");
    return ret_dummy;
  }
  
  status = napi_set_element(env, ret_arr, 2, ret_byte_size);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value ret_byte_size assign");
    return ret_dummy;
  }
  
  
  return ret_arr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//    encode
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct {
  u8 *data;
  unsigned long length;
  unsigned long position;
} Image_data;

napi_value jpeg_encode_rgb(napi_env env, napi_callback_info info) {
  napi_status status;
  
  napi_value ret_dummy;
  status = napi_create_int32(env, 0, &ret_dummy);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value ret_dummy");
    return ret_dummy;
  }
  
  size_t argc = 6;
  napi_value argv[6];
  status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Failed to parse arguments");
    return ret_dummy;
  }
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  u8 *data_src;
  size_t data_src_len;
  status = napi_get_buffer_info(env, argv[0], (void**)&data_src, &data_src_len);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Invalid buffer was passed as argument of data_src");
    return ret_dummy;
  }
  
  i32 size_x;
  status = napi_get_value_int32(env, argv[1], &size_x);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Invalid i32 was passed as argument of size_x");
    return ret_dummy;
  }
  
  i32 size_y;
  status = napi_get_value_int32(env, argv[2], &size_y);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Invalid i32 was passed as argument of size_y");
    return ret_dummy;
  }
  
  u8 *data_dst;
  size_t data_dst_len;
  status = napi_get_buffer_info(env, argv[3], (void**)&data_dst, &data_dst_len);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Invalid buffer was passed as argument of data_dst");
    return ret_dummy;
  }
  
  i32 data_dst_offset;
  status = napi_get_value_int32(env, argv[4], &data_dst_offset);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Invalid i32 was passed as argument of data_dst_offset");
    return ret_dummy;
  }
  
  i32 quality;
  status = napi_get_value_int32(env, argv[5], &quality);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Invalid i32 was passed as argument of quality");
    return ret_dummy;
  }
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  struct jpeg_compress_struct cinfo;
  struct my_jpeg_error_mgr jerr;
  
  JSAMPROW row_pointer[1];
  
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit=jpeg_cb_error_exit;
  
  if (setjmp(jerr.setjmp_buffer)) {
    jpeg_destroy_compress(&cinfo);
    napi_throw_error(env, NULL, "fail setjmp jerr.setjmp_buffer");
    return ret_dummy;
  }
  
  Image_data proxy;
  proxy.data = NULL;
  proxy.length = 0;
  proxy.position = 0;
  
  jpeg_create_compress(&cinfo);
  jpeg_mem_dest(&cinfo, (u8 **) &(proxy.data), &proxy.length);
  
  cinfo.image_width  = size_x;
  cinfo.image_height = size_y;
  cinfo.input_components = 3;
  cinfo.in_color_space = JCS_EXT_RGB;
  
  jpeg_set_defaults(&cinfo);
  
  jpeg_set_quality(&cinfo, quality, TRUE);
  
  jpeg_start_compress(&cinfo, TRUE);
  
  i32 line;
  while((line = cinfo.next_scanline) < size_y){
    row_pointer[0] = (JSAMPROW) (data_src + line * size_x * 3);
    (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }
  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);
  proxy.position = proxy.length;
  
  napi_value realloc_buffer;
  bool realloc = false;
  unsigned long len = proxy.position;
  if (len + data_dst_offset > data_dst_len) {
    realloc = true;
    data_dst_offset = 0;
    unsigned long new_len = 1;
    // slow but whatever
    while(len > new_len) {
      new_len <<= 1;
    }
    
    status = napi_create_buffer(env, new_len, (void**)(&data_dst), &realloc_buffer);
    if (status != napi_ok) {
      free(proxy.data);
      napi_throw_error(env, NULL, "fail napi_create_buffer");
      return ret_dummy;
    }
  }
  memcpy(data_dst + data_dst_offset, proxy.data, proxy.position);
  free(proxy.data);
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  napi_value ret_arr;
  status = napi_create_array(env, &ret_arr);
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value");
    return ret_dummy;
  }
  // data_dst_offset
  napi_value ret_data_dst_offset;
  status = napi_create_int32(env, data_dst_offset, &ret_data_dst_offset);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value ret_data_dst_offset");
    return ret_dummy;
  }
  
  status = napi_set_element(env, ret_arr, 0, ret_data_dst_offset);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value ret_data_dst_offset assign");
    return ret_dummy;
  }
  // ret_dst_write_size
  napi_value ret_data_dst_actual_size;
  status = napi_create_int32(env, len, &ret_data_dst_actual_size);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value ret_data_dst_actual_size");
    return ret_dummy;
  }
  
  status = napi_set_element(env, ret_arr, 1, ret_data_dst_actual_size);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value ret_dst_end_offset assign");
    return ret_dummy;
  }
  //
  
  if (realloc) {
    status = napi_set_element(env, ret_arr, 2, realloc_buffer);
    
    if (status != napi_ok) {
      napi_throw_error(env, NULL, "Unable to create return value ret_height assign");
      return ret_dummy;
    }
  } else {
    // argv[3] == data_dst
    // Я не знаю можно ли делать присваивание napi_value
    // Потому просто такой хардкод
    status = napi_set_element(env, ret_arr, 2, argv[3]);
    
    if (status != napi_ok) {
      napi_throw_error(env, NULL, "Unable to create return value ret_height assign");
      return ret_dummy;
    }
  }
  
  return ret_arr;
}

napi_value jpeg_encode_rgba(napi_env env, napi_callback_info info) {
  napi_status status;
  
  napi_value ret_dummy;
  status = napi_create_int32(env, 0, &ret_dummy);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value ret_dummy");
    return ret_dummy;
  }
  
  size_t argc = 6;
  napi_value argv[6];
  status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Failed to parse arguments");
    return ret_dummy;
  }
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  u8 *data_src;
  size_t data_src_len;
  status = napi_get_buffer_info(env, argv[0], (void**)&data_src, &data_src_len);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Invalid buffer was passed as argument of data_src");
    return ret_dummy;
  }
  
  i32 size_x;
  status = napi_get_value_int32(env, argv[1], &size_x);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Invalid i32 was passed as argument of size_x");
    return ret_dummy;
  }
  
  i32 size_y;
  status = napi_get_value_int32(env, argv[2], &size_y);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Invalid i32 was passed as argument of size_y");
    return ret_dummy;
  }
  
  u8 *data_dst;
  size_t data_dst_len;
  status = napi_get_buffer_info(env, argv[3], (void**)&data_dst, &data_dst_len);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Invalid buffer was passed as argument of data_dst");
    return ret_dummy;
  }
  
  i32 data_dst_offset;
  status = napi_get_value_int32(env, argv[4], &data_dst_offset);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Invalid i32 was passed as argument of data_dst_offset");
    return ret_dummy;
  }
  
  i32 quality;
  status = napi_get_value_int32(env, argv[5], &quality);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Invalid i32 was passed as argument of quality");
    return ret_dummy;
  }
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  struct jpeg_compress_struct cinfo;
  struct my_jpeg_error_mgr jerr;
  
  JSAMPROW row_pointer[1];
  
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit=jpeg_cb_error_exit;
  
  if (setjmp(jerr.setjmp_buffer)) {
    jpeg_destroy_compress(&cinfo);
    napi_throw_error(env, NULL, "fail setjmp jerr.setjmp_buffer");
    return ret_dummy;
  }
  
  Image_data proxy;
  proxy.data = NULL;
  proxy.length = 0;
  proxy.position = 0;
  
  jpeg_create_compress(&cinfo);
  jpeg_mem_dest(&cinfo, (u8 **) &(proxy.data), &proxy.length);
  
  cinfo.image_width  = size_x;
  cinfo.image_height = size_y;
  cinfo.input_components = 4;
  cinfo.in_color_space = JCS_EXT_RGBA;
  
  jpeg_set_defaults(&cinfo);
  
  jpeg_set_quality(&cinfo, quality, TRUE);
  
  jpeg_start_compress(&cinfo, TRUE);
  
  i32 line;
  while((line = cinfo.next_scanline) < size_y){
    row_pointer[0] = (JSAMPROW) (data_src + line * size_x * 4);
    (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }
  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);
  proxy.position = proxy.length;
  
  napi_value realloc_buffer;
  bool realloc = false;
  unsigned long len = proxy.position;
  if (len + data_dst_offset > data_dst_len) {
    realloc = true;
    data_dst_offset = 0;
    unsigned long new_len = 1;
    // slow but whatever
    while(len > new_len) {
      new_len <<= 1;
    }
    
    status = napi_create_buffer(env, new_len, (void**)(&data_dst), &realloc_buffer);
    if (status != napi_ok) {
      free(proxy.data);
      napi_throw_error(env, NULL, "fail napi_create_buffer");
      return ret_dummy;
    }
  }
  memcpy(data_dst + data_dst_offset, proxy.data, proxy.position);
  free(proxy.data);
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  napi_value ret_arr;
  status = napi_create_array(env, &ret_arr);
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value");
    return ret_dummy;
  }
  // data_dst_offset
  napi_value ret_data_dst_offset;
  status = napi_create_int32(env, data_dst_offset, &ret_data_dst_offset);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value ret_data_dst_offset");
    return ret_dummy;
  }
  
  status = napi_set_element(env, ret_arr, 0, ret_data_dst_offset);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value ret_data_dst_offset assign");
    return ret_dummy;
  }
  // ret_dst_write_size
  napi_value ret_data_dst_actual_size;
  status = napi_create_int32(env, len, &ret_data_dst_actual_size);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value ret_data_dst_actual_size");
    return ret_dummy;
  }
  
  status = napi_set_element(env, ret_arr, 1, ret_data_dst_actual_size);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value ret_dst_end_offset assign");
    return ret_dummy;
  }
  //
  
  if (realloc) {
    status = napi_set_element(env, ret_arr, 2, realloc_buffer);
    
    if (status != napi_ok) {
      napi_throw_error(env, NULL, "Unable to create return value ret_height assign");
      return ret_dummy;
    }
  } else {
    // argv[3] == data_dst
    // Я не знаю можно ли делать присваивание napi_value
    // Потому просто такой хардкод
    status = napi_set_element(env, ret_arr, 2, argv[3]);
    
    if (status != napi_ok) {
      napi_throw_error(env, NULL, "Unable to create return value ret_height assign");
      return ret_dummy;
    }
  }
  
  return ret_arr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

napi_value Init(napi_env env, napi_value exports) {
  napi_status status;
  napi_value fn;
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  status = napi_create_function(env, NULL, 0, jpeg_decode_rgba, NULL, &fn);
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to wrap native function");
  }
  
  status = napi_set_named_property(env, exports, "jpeg_decode_rgba", fn);
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to populate exports");
  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  status = napi_create_function(env, NULL, 0, jpeg_decode_rgb, NULL, &fn);
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to wrap native function");
  }
  
  status = napi_set_named_property(env, exports, "jpeg_decode_rgb", fn);
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to populate exports");
  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  status = napi_create_function(env, NULL, 0, jpeg_encode_rgb, NULL, &fn);
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to wrap native function");
  }
  
  status = napi_set_named_property(env, exports, "jpeg_encode_rgb", fn);
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to populate exports");
  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  status = napi_create_function(env, NULL, 0, jpeg_encode_rgba, NULL, &fn);
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to wrap native function");
  }
  
  status = napi_set_named_property(env, exports, "jpeg_encode_rgba", fn);
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to populate exports");
  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  status = napi_create_function(env, NULL, 0, jpeg_decode_size, NULL, &fn);
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to wrap native function");
  }
  
  status = napi_set_named_property(env, exports, "jpeg_decode_size", fn);
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to populate exports");
  }
  
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)