HDF5 "h5ex_t_regrefatt.h5" {
GROUP "/" {
   DATASET "DS1" {
      DATATYPE  H5T_STD_I32LE
      DATASPACE  NULL
      DATA {
      }
      ATTRIBUTE "A1" {
         DATATYPE  H5T_REFERENCE { H5T_STD_REF }
         DATASPACE  SIMPLE { ( 2 ) / ( 2 ) }
         DATA {
            DATASET "h5ex_t_regrefatt.h5/DS2"{
               REGION_TYPE POINT  (0,1), (2,11), (1,0), (2,4)
               DATATYPE  H5T_STD_I8LE
               DATASPACE  SIMPLE { ( 3, 16 ) / ( 3, 16 ) }
            }
            DATASET "h5ex_t_regrefatt.h5/DS2"  {
               REGION_TYPE BLOCK  (0,0)-(0,2), (0,11)-(0,13), (2,0)-(2,2),
                (2,11)-(2,13)
               DATATYPE  H5T_STD_I8LE
               DATASPACE  SIMPLE { ( 3, 16 ) / ( 3, 16 ) }
            }
         }
      }
   }
   DATASET "DS2" {
      DATATYPE  H5T_STD_I8LE
      DATASPACE  SIMPLE { ( 3, 16 ) / ( 3, 16 ) }
      DATA {
      (0,0): 84, 104, 101, 32, 113, 117, 105, 99, 107, 32, 98, 114, 111, 119,
      (0,14): 110, 0,
      (1,0): 102, 111, 120, 32, 106, 117, 109, 112, 115, 32, 111, 118, 101,
      (1,13): 114, 32, 0,
      (2,0): 116, 104, 101, 32, 53, 32, 108, 97, 122, 121, 32, 100, 111, 103,
      (2,14): 115, 0
      }
   }
}
}
