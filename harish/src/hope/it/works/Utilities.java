/**
 * @author Harish D
 */

package hope.it.works;

import java.io.BufferedReader;
import java.io.IOException;
import java.text.DecimalFormat;
import java.text.NumberFormat;
import java.util.StringTokenizer;

public class Utilities {
	
	public static int TRUE = 0;
	public static int FALSE = 1;
	public static int NOIDEA = 2;
	
	public static void pr(String s) {
		System.out.println(s);
	}
	
	public static String[] splitString(String s) {
		String[] ret = null;
		StringTokenizer tok = new StringTokenizer(s);
		ret = new String[tok.countTokens()];
		for (int i = 0; i < ret.length; i++) {
			ret[i] = tok.nextToken();
		}
		return ret;
	}
	
	public static void er(String s) {
		System.err.println("Error!!");
		System.err.println(s);
		System.exit(1);
	}
	
	public static String roundDouble(double num, int decimal) {
		String s = "0.";
		for (int i = 0; i < decimal; i++) {
			s += "0";
		}
		NumberFormat f = new DecimalFormat(s);
		s = f.format(num);
		return s;
	}
	
	public static int roundUp(int groupSize, int globalSize) {
        int r = globalSize % groupSize;
        if (r == 0) return globalSize;
        else        return globalSize + groupSize - r;
    }
	
	public static boolean isInteger(String fn) {
		try {
			Integer.parseInt(fn.trim());
			return true;
		} catch(Exception e) {
			return false;
		}
	}
	
	/**
	 * This function is used to split a String into a set of tokens seperated by
	 * a delimiter
	 * 
	 * @param s
	 * @param delimiter
	 * @return The array of tokens
	 */
	public static String[] splitString(String s, String delimiter) {
		String[] ret = null;
		StringTokenizer tok;

		if (delimiter == null || delimiter.length() == 0) {
			tok = new StringTokenizer(s);
		} else {
			tok = new StringTokenizer(s, delimiter);
		}

		ret = new String[tok.countTokens()];
		for (int i = 0; i < ret.length; i++) {
			ret[i] = tok.nextToken();
		}
		return ret;
	}

	public static String[] getLine(BufferedReader reader) throws IOException {
		String l = reader.readLine();
		if(l == null) {
			return null;
		}
		return splitString(l);
	}
	
	public static String[] getLine(BufferedReader reader, String delimiter) throws IOException {
		String l = reader.readLine();
		if(l == null) {
			return null;
		}
		return splitString(l,delimiter);
	}
	
	
	static double WORLD_ZOOM_LEVEL = 22;

	static double atanh(double x) { 
		return 0.5*Math.log( (1.0 + x) / (1.0 - x) ); 
	} 
	
	static double [] geo2world(double lat, double lon) {
	    double y;
	    if (lat==90.0)
	        y = 256;
	    else if (lat==-90.0)
	        y = 0;
	    else
	        y = (Math.PI - atanh( Math.sin(lat*Math.PI/180)))/Math.PI*128;
	    return new double [] {(lon+180)/360.0*256*Math.pow(2,WORLD_ZOOM_LEVEL), y*Math.pow(2,WORLD_ZOOM_LEVEL)};
	}

	static double[] world2geo(double x, double y) {
	    double s = Math.pow(2,WORLD_ZOOM_LEVEL);
	    return new double [] {Math.atan(Math.sinh(Math.PI*(1-y/s/128)))*180/Math.PI, x*360/s/256-18};
	}

	static double getGroundResolution(double lat, double lon) {
	    return Math.cos(lat * Math.PI/180) * 6378137 * 2 * Math.PI / Math.pow(2,8+WORLD_ZOOM_LEVEL);
	}
}