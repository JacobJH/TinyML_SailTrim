


    // !!! This file is generated using emlearn !!!

    #include <eml_trees.h>
    

static const EmlTreesNode random_forest_model_int_nodes[31] = {
  { 12, 0, 1, 2 },
  { 1, 0, -1, -2 },
  { 10, 0, -3, -2 },
  { 10, 0, -3, 1 },
  { 2, 0, -2, -1 },
  { 2, 0, 1, -1 },
  { 10, 0, -3, -2 },
  { 9, 0, 1, -1 },
  { 10, 0, -3, -2 },
  { 11, 0, 1, -1 },
  { 9, 0, -3, -2 },
  { 10, 0, -3, 1 },
  { 2, 0, -2, -1 },
  { 9, 0, 1, -1 },
  { 12, 0, -2, -3 },
  { 7, 0, 1, 2 },
  { 2, 0, -2, -1 },
  { 0, 1, -3, -2 },
  { 5, 0, 1, 2 },
  { 7, 0, -2, -3 },
  { 2, 0, -2, -1 },
  { 12, 0, 1, 2 },
  { 11, 0, -2, -1 },
  { 7, 1, -3, -2 },
  { 9, 0, 1, -1 },
  { 10, 0, -3, -2 },
  { 2, 0, 1, -1 },
  { 7, 0, -2, -3 },
  { 7, 0, 1, 2 },
  { 9, 0, -2, -1 },
  { 0, 1, -3, -2 } 
};

static const int32_t random_forest_model_int_tree_roots[13] = { 0, 3, 5, 7, 9, 11, 13, 15, 18, 21, 24, 26, 28 };

static const uint8_t random_forest_model_int_leaves[3] = { 2, 1, 0 };

EmlTrees random_forest_model_int = {
        31,
        (EmlTreesNode *)(random_forest_model_int_nodes),	  
        13,
        (int32_t *)(random_forest_model_int_tree_roots),
        3,
        (uint8_t *)(random_forest_model_int_leaves),
        0,
        14,
        3,
    };

static inline int32_t random_forest_model_int_tree_0(const int *features, int32_t features_length) {
          if (features[12] < 0) {
              if (features[1] < 0) {
                  return 2;
              } else {
                  return 1;
              }
          } else {
              if (features[10] < 0) {
                  return 0;
              } else {
                  return 1;
              }
          }
        }
        

static inline int32_t random_forest_model_int_tree_1(const int *features, int32_t features_length) {
          if (features[10] < 0) {
              return 0;
          } else {
              if (features[2] < 0) {
                  return 1;
              } else {
                  return 2;
              }
          }
        }
        

static inline int32_t random_forest_model_int_tree_2(const int *features, int32_t features_length) {
          if (features[2] < 0) {
              if (features[10] < 0) {
                  return 0;
              } else {
                  return 1;
              }
          } else {
              return 2;
          }
        }
        

static inline int32_t random_forest_model_int_tree_3(const int *features, int32_t features_length) {
          if (features[9] < 0) {
              if (features[10] < 0) {
                  return 0;
              } else {
                  return 1;
              }
          } else {
              return 2;
          }
        }
        

static inline int32_t random_forest_model_int_tree_4(const int *features, int32_t features_length) {
          if (features[11] < 0) {
              if (features[9] < 0) {
                  return 0;
              } else {
                  return 1;
              }
          } else {
              return 2;
          }
        }
        

static inline int32_t random_forest_model_int_tree_5(const int *features, int32_t features_length) {
          if (features[10] < 0) {
              return 0;
          } else {
              if (features[2] < 0) {
                  return 1;
              } else {
                  return 2;
              }
          }
        }
        

static inline int32_t random_forest_model_int_tree_6(const int *features, int32_t features_length) {
          if (features[9] < 0) {
              if (features[12] < 0) {
                  return 1;
              } else {
                  return 0;
              }
          } else {
              return 2;
          }
        }
        

static inline int32_t random_forest_model_int_tree_7(const int *features, int32_t features_length) {
          if (features[7] < 0) {
              if (features[2] < 0) {
                  return 1;
              } else {
                  return 2;
              }
          } else {
              if (features[0] < 1) {
                  return 0;
              } else {
                  return 1;
              }
          }
        }
        

static inline int32_t random_forest_model_int_tree_8(const int *features, int32_t features_length) {
          if (features[5] < 0) {
              if (features[7] < 0) {
                  return 1;
              } else {
                  return 0;
              }
          } else {
              if (features[2] < 0) {
                  return 1;
              } else {
                  return 2;
              }
          }
        }
        

static inline int32_t random_forest_model_int_tree_9(const int *features, int32_t features_length) {
          if (features[12] < 0) {
              if (features[11] < 0) {
                  return 1;
              } else {
                  return 2;
              }
          } else {
              if (features[7] < 1) {
                  return 0;
              } else {
                  return 1;
              }
          }
        }
        

static inline int32_t random_forest_model_int_tree_10(const int *features, int32_t features_length) {
          if (features[9] < 0) {
              if (features[10] < 0) {
                  return 0;
              } else {
                  return 1;
              }
          } else {
              return 2;
          }
        }
        

static inline int32_t random_forest_model_int_tree_11(const int *features, int32_t features_length) {
          if (features[2] < 0) {
              if (features[7] < 0) {
                  return 1;
              } else {
                  return 0;
              }
          } else {
              return 2;
          }
        }
        

static inline int32_t random_forest_model_int_tree_12(const int *features, int32_t features_length) {
          if (features[7] < 0) {
              if (features[9] < 0) {
                  return 1;
              } else {
                  return 2;
              }
          } else {
              if (features[0] < 1) {
                  return 0;
              } else {
                  return 1;
              }
          }
        }
        

int32_t random_forest_model_int_predict(const int *features, int32_t features_length) {

        int32_t votes[3] = {0,};
        int32_t _class = -1;

        _class = random_forest_model_int_tree_0(features, features_length); votes[_class] += 1;
    _class = random_forest_model_int_tree_1(features, features_length); votes[_class] += 1;
    _class = random_forest_model_int_tree_2(features, features_length); votes[_class] += 1;
    _class = random_forest_model_int_tree_3(features, features_length); votes[_class] += 1;
    _class = random_forest_model_int_tree_4(features, features_length); votes[_class] += 1;
    _class = random_forest_model_int_tree_5(features, features_length); votes[_class] += 1;
    _class = random_forest_model_int_tree_6(features, features_length); votes[_class] += 1;
    _class = random_forest_model_int_tree_7(features, features_length); votes[_class] += 1;
    _class = random_forest_model_int_tree_8(features, features_length); votes[_class] += 1;
    _class = random_forest_model_int_tree_9(features, features_length); votes[_class] += 1;
    _class = random_forest_model_int_tree_10(features, features_length); votes[_class] += 1;
    _class = random_forest_model_int_tree_11(features, features_length); votes[_class] += 1;
    _class = random_forest_model_int_tree_12(features, features_length); votes[_class] += 1;
    
        int32_t most_voted_class = -1;
        int32_t most_voted_votes = 0;
        for (int32_t i=0; i<3; i++) {

            if (votes[i] > most_voted_votes) {
                most_voted_class = i;
                most_voted_votes = votes[i];
            }
        }
        return most_voted_class;
    }
    