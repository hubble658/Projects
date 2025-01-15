package src;

import src.excepitons.DuplicateInfoException;
import src.excepitons.IDException;

import java.io.*;
import java.io.FileOutputStream;
import java.time.LocalDate;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.LinkedList;

public class CRS {
    private HashMap<Long,Patient> patients;
    private HashMap<Integer,Hospital> hospitals;
    private LinkedList<Rendezvous> rendezvous;

    public CRS() {
        this.patients = new HashMap<>();
        this.hospitals = new HashMap<>();
        this.rendezvous = new LinkedList<>();
    }

    public HashMap<Long, Patient> getPatients() {return patients;}
    public HashMap<Integer, Hospital> getHospitals() {return hospitals;}
    public LinkedList<Rendezvous> getRendezvous() {return rendezvous;}

    public boolean hasPatient(long patientId){

        boolean flag = false;
        for (long pKey: patients.keySet())
            if(pKey == patientId)
                flag = true;

        return flag;
    }
    public void updateRendezvous(){
        this.rendezvous = new LinkedList<>();
        for (Hospital h: hospitals.values())
            for (Section s : h.getSections())
                for (Doctor d: s.getDoctors())
                    rendezvous.addAll(d.getSchedule().getSessions());

        rendezvous.sort(Comparator.comparing(Rendezvous::getDateTime));
    }
    public synchronized void addPatient(Patient patient) throws DuplicateInfoException {
        for (long key: patients.keySet())
            if(key == patient.getId()){
                throw new DuplicateInfoException("Patient with this id already exists.");
            }
        patients.put(patient.getId(),patient);
    }
    public boolean removePatient(Patient patient){

        for (long key: patients.keySet())
            if(key == patient.getId()){
                patients.remove(key);
                for (Rendezvous r: rendezvous){
                    if(r.getPatient().getId() == patient.getId()){
                        r.getHospital().getSection(r.getSection().getId()).getDoctor(r.getDoctor().getDiplomaId()).getSchedule().removeRendezvous(r);
                    }
                }
                updateRendezvous();
                return true;
            }
        return false;
    }

    public boolean removeRendezvous(Rendezvous rendezvous){

        if(rendezvous.getDoctor().getSchedule().removeRendezvous(rendezvous)){
            updateRendezvous();
            return true;
        }
        return false;
    }

    public synchronized void addHospital(Hospital hospital) throws DuplicateInfoException{
        for (int key: hospitals.keySet())
            if(key == hospital.getId()){
                throw new DuplicateInfoException("Hospital with this id already exists.");
            }
        hospitals.put(hospital.getId(),hospital);
    }
    public boolean removeHospital(Hospital hospital){
        for (int key: hospitals.keySet())
            if(key == hospital.getId()){
                rendezvous.removeIf(r -> r.getHospital().getId() == hospital.getId());
                hospitals.remove(key);
                return true;
            }
        return false;
    }

    public synchronized boolean makeRendezvous(long patientId, int hospitalId, int sectionId, int diplomaId, LocalDate desiredDate) throws IDException {

        //check patient
        if(!hasPatient(patientId))
            throw new IDException("There is no Patient with this ID :"+ patientId);

        //check section
        if(hospitals.get(hospitalId) == null)
            throw new IDException("There is no Hospital with this ID :"+ hospitalId);
        if(!hospitals.get(hospitalId).hasSection(sectionId))
            throw new IDException("There is no Seciton with this ID :"+ sectionId);

        //check doctor
        if(!hospitals.get(hospitalId).getSection(sectionId).hasDoctor(diplomaId))
            throw new IDException("There is no Doctor with this ID :"+ diplomaId +" in section:"+ sectionId);

        //randevu eklemeye çalış
        Doctor   doctor   = hospitals.get(hospitalId).getSection(sectionId).getDoctor(diplomaId);
        Section  section  = hospitals.get(hospitalId).getSection(sectionId);
        Hospital hospital = hospitals.get(hospitalId);

        if(doctor.getSchedule().addRendezvous(patients.get(patientId),desiredDate, hospital , section , doctor)) {
            rendezvous.add(new Rendezvous(desiredDate, patients.get(patientId) , hospital ,section ,doctor));
            Collections.sort(rendezvous , (r1, r2) -> r1.getDateTime().compareTo(r2.getDateTime()));
            return true;
        }

        return false;
    }


    public void saveTablesToDisk(String fullPath){

        try {
            FileOutputStream fos = new FileOutputStream(fullPath);
            ObjectOutputStream oos = new ObjectOutputStream(fos);

            oos.writeObject(patients);
            oos.writeObject(hospitals);
            oos.writeObject(rendezvous);

            System.out.println("Serialization is completed succesfully");
        }catch(IOException ioe){
            ioe.printStackTrace();
        }

    }

    public void loadTablesFromDisk(String fullPath){

        try {
            FileInputStream fis = new FileInputStream(fullPath);
            ObjectInputStream ois = new ObjectInputStream(fis);

            patients = (HashMap<Long, Patient>) ois.readObject();
            hospitals = (HashMap<Integer, Hospital>) ois.readObject();
            rendezvous = (LinkedList<Rendezvous>) ois.readObject();

            System.out.println("Deserialization is completed succesfully");
        }catch(IOException | ClassNotFoundException e){
            e.printStackTrace();
        }

    }

    public void showALlInfo(){
        for (int hospitalId :hospitals.keySet()){
            Hospital h = hospitals.get(hospitalId);
            System.out.println("Hospital Name: "+h.getName() +"  Id:"+h.getId());
            h.showSections();
        }

        System.out.println("----");
        for (long key : patients.keySet())
            System.out.println(patients.get(key));
    }

    public void showRendezvous(){
        for (Rendezvous r: rendezvous)
            System.out.println(r);
    }

}
