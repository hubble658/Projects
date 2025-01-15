package src;

import src.excepitons.DuplicateInfoException;

import java.io.Serial;
import java.io.Serializable;
import java.util.LinkedList;

public class Section implements Serializable {

    @Serial
    private static final long serialVersionUID = 1005L;

    private final int id;
    private String name;
    private LinkedList<Doctor> doctors;

    public Section(String name, int id) {
        this.id = id;
        this.name = name;
        this.doctors = new LinkedList<Doctor>();
    }

    public int getId(){
        return id;
    }
    public String getName(){
        return name;
    }

    public void showDoctors(){
        for(Doctor d: doctors)
            System.out.println(" - " +d);
    }

    public Doctor getDoctor(int diploma_id){
        for (Doctor d:doctors){
            if(d.getDiplomaId()==diploma_id)
                return d;
        }
        return null;
    }
    public LinkedList<Doctor> getDoctors(){
        return doctors;
    }
    public boolean hasDoctor(int diploma_id){
        Doctor tmp;
        for (Doctor d: doctors)
            if(d.getDiplomaId() == diploma_id)
                return true;
        return  false;
    }

    public synchronized void addDoctor(Doctor newDoctor) throws DuplicateInfoException {

        for(Doctor d: doctors)
            if(d.getDiplomaId() == newDoctor.getDiplomaId())
                throw new DuplicateInfoException("Duplicate Info !!\nA doctor with same ID already exists.");

        doctors.add(newDoctor);
    }

    public boolean removeDoctor(Doctor newDoctor){
        for(Doctor d: doctors)
            if(d.getDiplomaId() == newDoctor.getDiplomaId()){
                doctors.remove(d);
                return true;
            }
        return false;
    }

    @Override
    public String toString(){
        return this.getName();
    }
}
