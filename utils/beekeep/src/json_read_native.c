#include <string.h>

#include "jansson.h"

#include "op.h"
#include "net_protected.h"
#include "param.h"
#include "preset.h"
#include "scene.h"

#include "json.h"

static void net_read_json_scene(json_t* o);
static void net_read_json_ops(json_t* o);
static void net_read_json_params(json_t* o);
static void net_read_json_presets(json_t* o);

// search for input idx, given op id and input name
static int search_op_input(int opIdx, const char* name);
// search for output idx, given op id and input name
static int search_op_output(int opIdx, const char* name);
// search for param idx, given name
static int search_param(const char* name);


void net_read_json_native(const char* name) {
  json_t *root;
  json_error_t err;
  FILE* f = fopen(name, "r");

  root = json_loadf(f, 0, &err);
  fclose(f);
  net_read_json_scene(json_object_get(root, "scene"));
  net_read_json_ops(json_object_get(root, "ops"));
  net_read_json_params(json_object_get(root, "params"));
  net_read_json_presets(json_object_get(root, "presets"));
}


static void net_read_json_scene(json_t* o) {
  json_t* p;
  strcpy(sceneData->desc.sceneName, json_string_value(json_object_get(o, "sceneName")));
  strcpy(sceneData->desc.moduleName, json_string_value(json_object_get(o, "moduleName")));
  // module version
  p = json_object_get(o, "moduleVersion");
  sceneData->desc.moduleVersion.maj = json_integer_value(json_object_get(p, "maj"));
  sceneData->desc.moduleVersion.min = json_integer_value(json_object_get(p, "min"));
  sceneData->desc.moduleVersion.rev = json_integer_value(json_object_get(p, "rev"));
  // bees version
  p = json_object_get(o, "beesVersion");
  sceneData->desc.beesVersion.maj = json_integer_value(json_object_get(p, "maj"));
  sceneData->desc.beesVersion.min = json_integer_value(json_object_get(p, "min"));
  sceneData->desc.beesVersion.rev = json_integer_value(json_object_get(p, "rev"));
 
}

static void net_read_json_ops(json_t* o) { 
  int count = json_array_size(o);
  /// binary state data..
  u8* src;
  u8 bin[0x10000];
  int binCount;
  int i, j;
  int id;
  op_t* op;
  
  // clear out any extant user ops in the network
  net_deinit();

  for( i=0; i<count; i++) {
    json_t* p = json_array_get(o, i);
    id = (op_id_t)json_integer_value(json_object_get(p, "class"));
    id = (op_id_t)json_integer_value(json_object_get(p, "type"));
    // add operator of indicated type
    net_add_op(id);
    // unpickle the state, if needed
    op = net->ops[net->numOps - 1];
    if(op->unpickle != NULL) {
      json_t* state = json_object_get(p, "state");
      binCount = json_array_size(state);
      for(j=0; j<binCount; j++) {
  	bin[j] = (u8)(json_integer_value(json_array_get(state, j)));
      }
      src = bin;
      src = (*(op->unpickle))(op, src);
      // sanity check
      if(binCount != ((size_t)src - (size_t)bin)) {
  	printf("warning! mis-sized byte array in operator state unpickle?");
  	printf("\r\n   bin: 0x%08x ; src: 0x%08x ", (unsigned int)bin, (unsigned int)src);
      }
    }

    /// set inputs and outputs!
    //...

  }
}

static void net_read_json_params(json_t* o) {
  int i;
  int v;
  pnode_t* param;
  int count = json_array_size( o );
  json_t* arr;

  net->numParams = count;
  arr = json_object_get(o, "data");
  for(i=0; i<count; i++) {
    json_t* p = json_array_get(o, i);
    param = &(net->params[i]);
    param->idx = json_integer_value(json_object_get(p, "idx"));
    param->data.value = json_integer_value(json_object_get(p, "value"));
    param->desc.type = json_integer_value(json_object_get(p, "type"));
    param->desc.min = json_integer_value(json_object_get(p, "min"));
    param->desc.max = json_integer_value(json_object_get(p, "max"));
    param->desc.radix = json_integer_value(json_object_get(p, "radix"));
    strcpy(param->desc.label, json_string_value(json_object_get(p, "label")));
  }
}

static void net_read_json_presets(json_t* o) {
  int count = json_array_size( o );
  int n;
  int i, j;
  char* p;
  // sanity check
  if( count != NET_PRESETS_MAX) {
    printf(" \n warning! preset array size in json does not match network preset count.\n");
  }

  // empty out extant preset data
  for(i=0; i<NET_PRESETS_MAX; i++) {

    // empty name
    for(j=0; j<PRESET_NAME_LEN; j++) {
      presets[i].name[j] = '\0';
    }

    //    p = presets[i].name;
    //    p = atoi_idx(p, i);
    //    *p = '_';

    // empty inputs
    for(j=0; j<PRESET_INODES_COUNT; ++j) {
      presets[i].ins[j].value = 0;
      presets[i].ins[j].enabled = 0;
    }
    // empty outputs
    for(j=0; j<NET_OUTS_MAX; ++j) {
      presets[i].outs[j].target = -1;
      presets[i].outs[j].enabled = 0;
    }
  }

  for(i=0; i<count; ++i) {
    json_t* p = json_array_get(o, i);
    json_t* arr = json_object_get(p, "entries");
    strcpy(presets[i].name, json_string_value(json_object_get(p, "name")));
    n = json_array_size(arr);

    for(j=0; j<n; ++j) {
      json_t* q = json_array_get(arr, j);
      json_t* r;
      json_t* s;
      int opIdx;
      int opInIdx, inIdx;
      int opOutIdx, outIdx;
      char name[32];
      /*
	ok... here we need to do some parsing stuff,
        because there are a number of possible formats for preset entries.

	initially, let's just assume the format used in beekeep:json_write_native()
	
       */

      // input node preset entry
      r = json_object_get(q, "inIdx");
      if(r != NULL) {
	/// this is an input node with raw input
	// set value
	inIdx = json_integer_value(r);
	presets[i].ins[inIdx].value = json_integer_value(json_object_get(q, "value"));
	presets[i].ins[inIdx].enabled = 1;
	continue;
      }
      r = json_object_get(q, "opInName");
      if(r != NULL) {
	/// this is an input node with input name and op idx
	r = json_object_get(q, "opIdx");
	if(r == NULL) { 
	  printf("error parsing preset entry (input), preset no. %d", i);
	  continue;
	}
	opIdx = json_integer_value(r);
	r = json_object_get(q, "opInName");
	if(r == NULL) { 
	  printf("error parsing preset entry (input), preset no. %d", i);
	  continue;
	}
	strcpy(name, json_string_value(r) );
	opInIdx = search_op_input(opIdx, name);
	if(opInIdx == -1) { 
	  printf("error parsing preset entry (input), preset no. %d", i);
	  continue;
	}
	// set value
	r = json_object_get(q, "value");
	inIdx = net_op_in_idx(opIdx, opInIdx);
	presets[i].ins[inIdx].value = json_integer_value(r);
	continue;
      }

      r = json_object_get(q, "opOutName");
      if(r != NULL) {
	/// this is an output node with output name and op idx
	strcpy(name, json_string_value(r) );
	opIdx = json_integer_value( json_object_get(q, "opIdx") );
	opOutIdx = search_op_output(opIdx, name);
	outIdx = net_op_out_idx(opIdx, opOutIdx);
	// get target data...
	r = json_object_get(q, "target");
	s = json_object_get(r, "paramName");
	if(s != NULL) {
	  // param target
	  strcpy(name, json_string_value(s) );
	  inIdx = search_param(name);
	} else {
	  // op input target
	  s = json_object_get(r, "opIdx");
	  opIdx = json_integer_value(s);
	  s = json_object_get(r, "opInName");
	  strcpy(name, json_string_value(s) );
	  inIdx = search_op_input(opIdx, name);
	}
	if(inIdx == -1) { printf("error parsing target in preset %d", i); continue; }
	presets[i].outs[outIdx].target = inIdx;
	presets[i].outs[outIdx].enabled = 1;
	continue;
      }
      r = json_object_get(q, "paramName");
      if(r != NULL) {
	/// this is a param entry
	strcpy(name, json_string_value(r));
	inIdx = search_param(name);
	r = json_object_get(q, "value");
	presets[i].ins[inIdx].value = json_integer_value(r);
	presets[i].ins[inIdx].enabled = 1;
	continue;
      }
    }
  }
}

// search for input idx, given op id and input name
int search_op_input(int opIdx, const char* name) {
  int i;
  op_t* op = net->ops[opIdx];
  for(i=0; i < op->numInputs; ++i) {
    if(strcmp(op_in_name(op, i), name) == 0) {
      return i;
    }
  }
  return -1;
}

// search for output idx, given op id and input name
int search_op_output(int opIdx, const char* name) {
  int i;
  op_t* op = net->ops[opIdx];
  for(i=0; i < op->numOutputs; ++i) {
    if(strcmp(op_out_name(op, i), name) == 0) {
      return i;
    }
  }
  return -1;
}

// search for param idx (in inputs list), given name
static int search_param(const char* name) {
  int i;
  for(i=0; i < net_num_params(); ++i) {
    if( strcmp( get_param_name(i), name) == 0) {
      // offset into global input list
      return i + net->numIns;
    }
  }
  return -1;
}
