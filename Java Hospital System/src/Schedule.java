package src;

import java.io.Serial;
import java.io.Serializable;
import java.time.LocalDate;
import java.util.LinkedList;
import java.util.List;

public class Schedule implements Serializable {

    @Serial
    private static final long serialVersionUID = 1004L;

    private List<Rendezvous> sessions;
    private int maxPatientPerDay;

    public Schedule(int maxPatientPerDay){
        this.maxPatientPerDay = maxPatientPerDay;
        this.sessions = new LinkedList<Rendezvous>();
    }

    public boolean addRendezvous(Patient patient, LocalDate desiredDate , Hospital hospital, Section section, Doctor doctor){

        int rCount = 0;
        Rendezvous newRendezvous = new Rendezvous(desiredDate, patient, hospital, section, doctor);

        for (Rendezvous r: sessions)
            if(r.isSameDay(newRendezvous))
                rCount++;

        if(rCount < this.maxPatientPerDay){
            this.sessions.add(newRendezvous);
            return true;
        }

        return false;
    }

    public boolean addRendezvous(Rendezvous newRendezvous){

        int rCount = 0;
        for (Rendezvous r: sessions)
            if(r.isSameDay(newRendezvous))
                rCount++;

        if(rCount < this.maxPatientPerDay){
            this.sessions.add(newRendezvous);
            return true;
        }

        return false;
    }

    public boolean removeRendezvous(Rendezvous rendezvous){
        return sessions.remove(rendezvous);
    }

    public List<Rendezvous> getSessions(){
        return sessions;
    }

    public void showRendezvous(){
        for(Rendezvous r: sessions)
            System.out.println(r);
    }
}

