package src.testPackage;

import org.junit.jupiter.api.Test;
import src.Patient;

import static org.junit.jupiter.api.Assertions.*;

class PatientTest {

    Patient tmpPatient1 = new Patient("Test Patient", 12654897);
    Patient tmpPatient2 = new Patient("Test Patient", 12654897);

    @Test
    void doesEqual(){
        assertEquals(tmpPatient1.getId(), (tmpPatient2.getId()));
    }
    @Test
    void toStringTest(){
        assertEquals(tmpPatient1.toString(), "Test Patient 12***");
    }

}