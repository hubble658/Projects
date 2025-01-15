package src;

import src.excepitons.DuplicateInfoException;

import java.io.Serial;
import java.io.Serializable;
import java.util.LinkedList;

public class Hospital implements Serializable {

    @Serial
    private static final long serialVersionUID = 1002L;

    private final int id;
    private String name;
    private LinkedList<Section> sections;


    public Hospital(String name ,int id){
        this.id = id;
        this.name = name;
        this.sections = new LinkedList<Section>();
    }

    public int getId() {
        return id;
    }
    public String getName(){
        return name;
    }
    public LinkedList<Section> getSections(){
        return sections;
    }
    public Section getSection(int id){
        for(Section s: sections)
            if(s.getId() == id)
                return s;
        return null;
    }

    public Section getSection(String name){
        for(Section s: sections)
            if(s.getName().equalsIgnoreCase(name))
                return s;
        return null;
    }

    public synchronized void addSection(Section section) throws DuplicateInfoException {
        boolean flag;
        for (Section s: sections)
            if(s.getId()== section.getId())
                throw new DuplicateInfoException("Section with this ID already exists.");

        sections.add(section);
    }

    public boolean removeSection(Section section) {
        boolean flag;
        for (Section s: sections)
            if(s.getId()== section.getId()){
                sections.remove(s);
                return true;
            }
        return false;
    }
    public boolean hasDoctor(int diploma_id){
        Doctor tmp;
        for (Section s: sections) {
            tmp = s.getDoctor(diploma_id);
            if(tmp != null)
                return true;
        }
        return  false;
    }
    public boolean hasSection(int sectionId){
        for (Section s: sections) {
            if(s.getId() == sectionId)
                return true;
        }
        return  false;
    }

    public void showSections(){
        for (Section s: sections){
            System.out.println("Section: "+ s.getName());
            s.showDoctors();
        }
    }

    @Override
    public String toString(){
        return this.getName();
    }

}
