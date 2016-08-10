/**
 * 
 */
package com.ibm.in.irl.st.anomaly.parser;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.InputStreamReader;

import com.ibm.in.irl.st.anomaly.data.GridData;

/**
 * InputFileParser.java
 * <p/>
 * Purpose: Parses the input file and stores the data values in a
 * {@link GridData} structure.
 */
public class InputFileParser {
	/**
	 * The field stored the reader object
	 */
	private BufferedReader reader = null;

	/**
	 * The file to be parsed
	 */
	private String fileName = null;

	/**
	 * The field states whether the variables are initialized
	 */
	private boolean initialized = false;

	/**
	 * The gini is calculated on positive values. This field stores the bias
	 * value to be added to each entry
	 */
	private double VALUE_BIAS = 0.0;

	/**
	 * The field stored the value to be adjusted in order to fit the row number
	 * as matrix index
	 */
	private double ROW_BIAS = 0.0;

	/**
	 * The field stored the value to be adjusted in order to fit the column
	 * number as matrix index
	 */
	private double COL_BIAS = 0.0;

	public InputFileParser(String fileName, double rowBias, double colBias, double valueBias) throws Exception {
		this.fileName = fileName;
		VALUE_BIAS = valueBias;
		ROW_BIAS = rowBias;
		COL_BIAS = colBias;
		initialize(fileName);
	}

	private void initialize(String fileName) throws Exception {
		//####// Reads the input file
		reader = new BufferedReader(new InputStreamReader(new FileInputStream(fileName)));
		initialized = true;
	}

	public void close() throws Exception {
		if (reader != null) {
			reader.close();
		}
	}

	/**
	 * getGridData
	 * @return Purpose: Populates the grid given the input data. This method
	 * will change if the input data format is also changed.
	 * 
	 * ##### TO BE ADAPTED
	 * 
	 */
	public void fillGridData(GridData grid) throws Exception {
		if (!initialized) {
			initialize(fileName);
		}
		String line = null;
		String[] tokens = null;
		int row = 0;
		int col = 0;
		double temperature = 0.0;
		while ((line = reader.readLine()) != null) {
			tokens = line.split("\\s");
			//Each line contains the long, lat and annual temperature
			if (tokens.length < 3) {
				continue;
			}
			col = (int) ((Double.parseDouble(tokens[0]) + COL_BIAS) * 25 / 3);
			row = (int) ((Double.parseDouble(tokens[1]) + ROW_BIAS) * 25 / 3);
			for (int interval = 0; interval < grid.getIntervals(); interval++) {
				temperature = Double.parseDouble(tokens[2 + interval]) + VALUE_BIAS;
				grid.setValue(interval, row, col, temperature);
			}

		}
	}

	/*	*//**
	 * fileRandomData
	 * @param grid Purpose:
	 */
	/*
	 * public void fileRandomData(GridData grid) { int cols = grid.getCols();
	 * int rows = grid.getRows(); double value = 0; for (int i = 0; i < rows;
	 * i++) { for (int j = 0; j < cols; j++) { value = Math.random() * 100;
	 * grid.setValue(i, j, value); } } }
	 *//**
	 * loadGrid
	 * @param grid Purpose:
	 */

	public void loadGrid(GridData grid, String fieldSeparator) throws Exception {
		if (!initialized) {
			initialize(fileName);
		}
		String line = null;
		String[] tokens = null;
		int row = 0;
		int col = 0;
		double value = 0;
		while ((line = reader.readLine()) != null) {
			tokens = line.split(fieldSeparator);
			for (col = 0; col < tokens.length; col++) {
				value = Double.parseDouble(tokens[col]);
				if (value != -1) {
					grid.setValue(row / grid.getRows(), row % grid.getRows(), col, value);
				}
			}
			row++;
		}
	}
	/**
	 * fillPointData
	 * @param grid</p>
	 * <p>
	 * <b>Purpose:<b/>
	 * </p>
	 * @throws Exception
	 */
	/*
	 * public void fillPointData(GridData grid) throws Exception { if
	 * (!initialized) { initialize(fileName); } String line = null; String[]
	 * tokens = null; int row = 0; int col = 0; while ((line =
	 * reader.readLine()) != null) { System.out.println(line); tokens =
	 * line.split(","); if (tokens.length < 3) { continue; } row = (int)
	 * ((Double.parseDouble(tokens[1]) + ROW_BIAS) * 100); col = (int)
	 * ((Double.parseDouble(tokens[2]) + COL_BIAS) * 100);
	 * System.out.println(row + " " + col); //id =
	 * Double.parseDouble(tokens[0]); grid.addValue(row, col); } }
	 */
}
