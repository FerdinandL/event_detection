package hope.it.works;

public class Test {

	
	double first = 0;
	double second = 0;
	
	public void setFirst(int number){
		double n = (double)number;
		this.first = 0.0004*number/50.0;
	}
	
	public static void main(String[] args) {
		
		Test t = new Test();
		t.setFirst(110);
		System.out.println(t.first);
	}
}
