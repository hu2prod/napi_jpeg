#include <node_api.h>

#include <stdio.h>
#include <setjmp.h>
#include <jpeglib.h>

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
  unsigned char *data_src;
  size_t data_src_len;
  status = napi_get_buffer_info(env, argv[0], (void**)&data_src, &data_src_len);
  
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Invalid buffer was passed as argument of data_src");
    return ret_dummy;
  }
  
  unsigned char *data_dst;
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
    napi_throw_error(env, NULL, "fail");
    return ret_dummy;
  }
  
  jpeg_create_decompress(&cinfo);
  jpeg_mem_src(&cinfo, (unsigned char *) data_src, data_src_len);  
  jpeg_read_header(&cinfo, TRUE);
  cinfo.out_color_space = JCS_EXT_RGBA;
  jpeg_start_decompress(&cinfo);
  
  unsigned int width, height, line;
  
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
    unsigned char *ptr = data_dst + 4*line*width;
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

napi_value Init(napi_env env, napi_value exports) {
  napi_status status;
  napi_value fn;
  
  status = napi_create_function(env, NULL, 0, jpeg_decode_rgba, NULL, &fn);
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to wrap native function");
  }
  
  status = napi_set_named_property(env, exports, "jpeg_decode_rgba", fn);
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to populate exports");
  }
  
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)