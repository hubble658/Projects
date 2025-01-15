package src;

public class Doctor extends Person{

    private final int diplomaId;
    private Schedule schedule;

    public Doctor(String name, long national_id, int diplomaId, int maxPatientPerDay) {
        super(name, national_id);
        this.diplomaId = diplomaId;
        this.schedule = new Schedule(maxPatientPerDay);
    }

    public Doctor(String name, long national_id, int diplomaId) {
        super(name, national_id);
        this.diplomaId = diplomaId;
        this.schedule = new Schedule(15);
    }


    public int getDiplomaId() {
        return diplomaId;
    }

    public Schedule getSchedule(){
        return schedule;
    }

    @Override
    public String toString(){
        return "Doctor: "+ super.getName();
    }
}
