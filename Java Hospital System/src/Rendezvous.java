package src;

import java.io.Serial;
import java.io.Serializable;
import java.time.LocalDate;
import java.time.format.DateTimeFormatter;

public class Rendezvous implements Serializable {

    @Serial
    private static final long serialVersionUID = 1003L;
    private static DateTimeFormatter dtf = DateTimeFormatter.ofPattern("dd-MM-yyyy");

    private LocalDate dateTime;
    private Patient patient;
    private Hospital hospital;
    private Section section;
    private Doctor doctor;


    public Rendezvous(LocalDate dateTime, Patient patient, Hospital hospital, Section section, Doctor doctor) {
        this.dateTime = dateTime;
        this.patient = patient;
        this.hospital = hospital;
        this.section = section;
        this.doctor = doctor;
    }

    public Patient getPatient() {
        return patient;
    }

    public LocalDate getDateTime() {
        return dateTime;
    }

    public Hospital getHospital() {
        return hospital;
    }

    public Section getSection() {
        return section;
    }

    public Doctor getDoctor() {
        return doctor;
    }

    public boolean isSameDay(Rendezvous r){
        return this.dateTime.getDayOfYear() == r.dateTime.getDayOfYear();
    }

    @Override
    public String toString(){
        return  dtf.format(dateTime) +" Doctor: " + doctor.getName()+" / " + section.getName()+" / " + hospital.getName();
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true; // Aynı nesne kontrolü
        if (obj == null || getClass() != obj.getClass()) return false; // Null veya farklı sınıf kontrolü

        Rendezvous that = (Rendezvous) obj;

        // dateTime, patient, hospital ve doctor id'lerini karşılaştırıyoruz
        return this.isSameDay(that) &&
                this.patient.getId() == (that.patient.getId()) &&
                this.hospital.getId() == (that.hospital.getId()) &&
                this.doctor.getId() == (that.doctor.getId());
    }


}
