package src.testPackage;

import org.junit.jupiter.api.Test;
import src.Person;

import static org.junit.jupiter.api.Assertions.*;

class PersonTest {

    Person person1 = new Person("Test Person1",1233242);
    Person person2 = new Person("Test Person2",6871248);

    @Test
    void getName() {
        assertEquals(person1.getName(),"Test Person1");
    }

    @Test
    void doesEqual() {
        assertNotEquals(person1,person2);
    }

}