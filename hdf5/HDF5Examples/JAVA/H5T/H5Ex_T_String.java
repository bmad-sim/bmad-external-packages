/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright by The HDF Group.                                               *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of HDF5.  The full HDF5 copyright notice, including     *
 * terms governing use, modification, and redistribution, is contained in    *
 * the COPYING file, which can be found at the root of the source code       *
 * distribution tree, or in https://www.hdfgroup.org/licenses.               *
 * If you do not have access to either file, you may request a copy from     *
 * help@hdfgroup.org.                                                        *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/************************************************************
  This example shows how to read and write string datatypes
  to a dataset.  The program first writes strings to a
  dataset with a dataspace of DIM0, then closes the file.
  Next, it reopens the file, reads back the data, and
  outputs it to the screen.
 ************************************************************/

import hdf.hdf5lib.H5;
import hdf.hdf5lib.HDF5Constants;

public class H5Ex_T_String {
    private static String FILENAME    = "H5Ex_T_String.h5";
    private static String DATASETNAME = "DS1";
    private static final int DIM0     = 4;
    private static final int SDIM     = 8;
    private static final int RANK     = 1;

    private static void CreateDataset()
    {
        long file_id            = HDF5Constants.H5I_INVALID_HID;
        long memtype_id         = HDF5Constants.H5I_INVALID_HID;
        long filetype_id        = HDF5Constants.H5I_INVALID_HID;
        long dataspace_id       = HDF5Constants.H5I_INVALID_HID;
        long dataset_id         = HDF5Constants.H5I_INVALID_HID;
        long[] dims             = {DIM0};
        byte[][] dset_data      = new byte[DIM0][SDIM];
        StringBuffer[] str_data = {new StringBuffer("Parting"), new StringBuffer("is such"),
                                   new StringBuffer("sweet"), new StringBuffer("sorrow.")};

        // Create a new file using default properties.
        try {
            file_id = H5.H5Fcreate(FILENAME, HDF5Constants.H5F_ACC_TRUNC, HDF5Constants.H5P_DEFAULT,
                                   HDF5Constants.H5P_DEFAULT);
        }
        catch (Exception e) {
            e.printStackTrace();
        }

        // Create file and memory datatypes. For this example we will save
        // the strings as FORTRAN strings, therefore they do not need space
        // for the null terminator in the file.
        try {
            filetype_id = H5.H5Tcopy(HDF5Constants.H5T_FORTRAN_S1);
            if (filetype_id >= 0)
                H5.H5Tset_size(filetype_id, SDIM - 1);
        }
        catch (Exception e) {
            e.printStackTrace();
        }
        try {
            memtype_id = H5.H5Tcopy(HDF5Constants.H5T_C_S1);
            if (memtype_id >= 0)
                H5.H5Tset_size(memtype_id, SDIM);
        }
        catch (Exception e) {
            e.printStackTrace();
        }

        // Create dataspace. Setting maximum size to NULL sets the maximum
        // size to be the current size.
        try {
            dataspace_id = H5.H5Screate_simple(RANK, dims, null);
        }
        catch (Exception e) {
            e.printStackTrace();
        }

        // Create the dataset and write the string data to it.
        try {
            if ((file_id >= 0) && (filetype_id >= 0) && (dataspace_id >= 0))
                dataset_id =
                    H5.H5Dcreate(file_id, DATASETNAME, filetype_id, dataspace_id, HDF5Constants.H5P_DEFAULT,
                                 HDF5Constants.H5P_DEFAULT, HDF5Constants.H5P_DEFAULT);
        }
        catch (Exception e) {
            e.printStackTrace();
        }

        // Write the data to the dataset.
        try {
            for (int indx = 0; indx < DIM0; indx++) {
                for (int jndx = 0; jndx < SDIM; jndx++) {
                    if (jndx < str_data[indx].length())
                        dset_data[indx][jndx] = (byte)str_data[indx].charAt(jndx);
                    else
                        dset_data[indx][jndx] = 0;
                }
            }
            if ((dataset_id >= 0) && (memtype_id >= 0))
                H5.H5Dwrite(dataset_id, memtype_id, HDF5Constants.H5S_ALL, HDF5Constants.H5S_ALL,
                            HDF5Constants.H5P_DEFAULT, dset_data);
        }
        catch (Exception e) {
            e.printStackTrace();
        }

        // End access to the dataset and release resources used by it.
        try {
            if (dataset_id >= 0)
                H5.H5Dclose(dataset_id);
        }
        catch (Exception e) {
            e.printStackTrace();
        }

        // Terminate access to the data space.
        try {
            if (dataspace_id >= 0)
                H5.H5Sclose(dataspace_id);
        }
        catch (Exception e) {
            e.printStackTrace();
        }

        // Terminate access to the file type.
        try {
            if (filetype_id >= 0)
                H5.H5Tclose(filetype_id);
        }
        catch (Exception e) {
            e.printStackTrace();
        }

        // Terminate access to the mem type.
        try {
            if (memtype_id >= 0)
                H5.H5Tclose(memtype_id);
        }
        catch (Exception e) {
            e.printStackTrace();
        }

        // Close the file.
        try {
            if (file_id >= 0)
                H5.H5Fclose(file_id);
        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }

    private static void ReadDataset()
    {
        long file_id      = HDF5Constants.H5I_INVALID_HID;
        long filetype_id  = HDF5Constants.H5I_INVALID_HID;
        long memtype_id   = HDF5Constants.H5I_INVALID_HID;
        long dataspace_id = HDF5Constants.H5I_INVALID_HID;
        long dataset_id   = HDF5Constants.H5I_INVALID_HID;
        long sdim         = 0;
        long[] dims       = {DIM0};
        byte[][] dset_data;
        StringBuffer[] str_data;

        // Open an existing file.
        try {
            file_id = H5.H5Fopen(FILENAME, HDF5Constants.H5F_ACC_RDONLY, HDF5Constants.H5P_DEFAULT);
        }
        catch (Exception e) {
            e.printStackTrace();
        }

        // Open an existing dataset.
        try {
            if (file_id >= 0)
                dataset_id = H5.H5Dopen(file_id, DATASETNAME, HDF5Constants.H5P_DEFAULT);
        }
        catch (Exception e) {
            e.printStackTrace();
        }

        // Get the datatype and its size.
        try {
            if (dataset_id >= 0)
                filetype_id = H5.H5Dget_type(dataset_id);
            if (filetype_id >= 0) {
                sdim = H5.H5Tget_size(filetype_id);
                sdim++; // Make room for null terminator
            }
        }
        catch (Exception e) {
            e.printStackTrace();
        }

        // Get dataspace and allocate memory for read buffer.
        try {
            if (dataset_id >= 0)
                dataspace_id = H5.H5Dget_space(dataset_id);
        }
        catch (Exception e) {
            e.printStackTrace();
        }

        try {
            if (dataspace_id >= 0)
                H5.H5Sget_simple_extent_dims(dataspace_id, dims, null);
        }
        catch (Exception e) {
            e.printStackTrace();
        }

        // Allocate space for data.
        dset_data = new byte[(int)dims[0]][(int)sdim];
        str_data  = new StringBuffer[(int)dims[0]];

        // Create the memory datatype.
        try {
            memtype_id = H5.H5Tcopy(HDF5Constants.H5T_C_S1);
            if (memtype_id >= 0)
                H5.H5Tset_size(memtype_id, sdim);
        }
        catch (Exception e) {
            e.printStackTrace();
        }

        // Read data.
        try {
            if ((dataset_id >= 0) && (memtype_id >= 0))
                H5.H5Dread(dataset_id, memtype_id, HDF5Constants.H5S_ALL, HDF5Constants.H5S_ALL,
                           HDF5Constants.H5P_DEFAULT, dset_data);
            byte[] tempbuf = new byte[(int)sdim];
            for (int indx = 0; indx < (int)dims[0]; indx++) {
                for (int jndx = 0; jndx < sdim; jndx++) {
                    tempbuf[jndx] = dset_data[indx][jndx];
                }
                str_data[indx] = new StringBuffer(new String(tempbuf).trim());
            }
        }
        catch (Exception e) {
            e.printStackTrace();
        }

        // Output the data to the screen.
        for (int indx = 0; indx < dims[0]; indx++) {
            System.out.println(DATASETNAME + " [" + indx + "]: " + str_data[indx]);
        }
        System.out.println();

        // End access to the dataset and release resources used by it.
        try {
            if (dataset_id >= 0)
                H5.H5Dclose(dataset_id);
        }
        catch (Exception e) {
            e.printStackTrace();
        }

        // Terminate access to the data space.
        try {
            if (dataspace_id >= 0)
                H5.H5Sclose(dataspace_id);
        }
        catch (Exception e) {
            e.printStackTrace();
        }

        // Terminate access to the file type.
        try {
            if (filetype_id >= 0)
                H5.H5Tclose(filetype_id);
        }
        catch (Exception e) {
            e.printStackTrace();
        }

        // Terminate access to the mem type.
        try {
            if (memtype_id >= 0)
                H5.H5Tclose(memtype_id);
        }
        catch (Exception e) {
            e.printStackTrace();
        }

        // Close the file.
        try {
            if (file_id >= 0)
                H5.H5Fclose(file_id);
        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static void main(String[] args)
    {
        H5Ex_T_String.CreateDataset();
        // Now we begin the read section of this example. Here we assume
        // the dataset and array have the same name and rank, but can have
        // any size. Therefore we must allocate a new array to read in
        // data using malloc().
        H5Ex_T_String.ReadDataset();
    }
}
