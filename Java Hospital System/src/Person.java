package src;

import java.io.Serial;
import java.io.Serializable;

public class Person implements Serializable {

    @Serial
    private static final long serialVersionUID = 1001L;

    private String name;
    private long national_id;

    public Person(String name, long national_id){
        this.name = name;
        this.national_id = national_id;
    }

    public String getName(){
        return name;
    }
    public long getId(){
        return national_id;
    }

    @Override
    public String toString(){
        return "Person Name: "+this.name;
    }

}
