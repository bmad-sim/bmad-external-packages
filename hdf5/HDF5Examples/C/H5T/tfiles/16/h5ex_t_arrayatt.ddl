HDF5 "h5ex_t_arrayatt.h5" {
GROUP "/" {
   DATASET "DS1" {
      DATATYPE  H5T_STD_I32LE
      DATASPACE  SCALAR
      DATA {
      (0): 0
      }
      ATTRIBUTE "A1" {
         DATATYPE  H5T_ARRAY { [3][5] H5T_STD_I64LE }
         DATASPACE  SIMPLE { ( 4 ) / ( 4 ) }
         DATA {
         (0): [ 0, 0, 0, 0, 0,
               0, -1, -2, -3, -4,
               0, -2, -4, -6, -8 ],
         (1): [ 0, 1, 2, 3, 4,
               1, 1, 1, 1, 1,
               2, 1, 0, -1, -2 ],
         (2): [ 0, 2, 4, 6, 8,
               2, 3, 4, 5, 6,
               4, 4, 4, 4, 4 ],
         (3): [ 0, 3, 6, 9, 12,
               3, 5, 7, 9, 11,
               6, 7, 8, 9, 10 ]
         }
      }
   }
}
}
