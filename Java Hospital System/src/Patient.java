package src;

public class Patient extends Person{

    public Patient(String name, long national_id) {super(name, national_id);}

    @Override
    public String toString(){
        if(this.getId()<100)
            return  this.getName() +" " + String.valueOf(this.getId());
        return  this.getName() +" " + String.valueOf(this.getId()).substring(0,2)+"***";
    }
}
