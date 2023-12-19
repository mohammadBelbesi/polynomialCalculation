program name: polynomCalc
student name: mohammad belbesi

==Description:==
*in polynomCalc we Calculation of a polynom expression
-----------------------------------
in this section its performs polynomial operations such as addition, subtraction, and multiplication. The program allows the user to input polynomial operations in a specific format and then performs the requested operation.
-----------------------------------
*in polynomCalc2a and polynomCalc2b we Calculation of a polynom expression by using two programs "processes"
in this section its performs polynomial operations such as addition, subtraction, and multiplication. The program allows the user to input polynomial operations in a specific format and then performs the requested operation by using two programs , the first is the consumer "producer" and the second is the customer , the two programs are using a shared memory in bounded buffer way and for solving the synchronizing program we use mutex and semaphore.
-----------------------------------
*in polynomCalc3a and polynomCalc3b we Calculation of a polynom expression by using two programs and "threads"
in this section its performs polynomial operations such as addition, subtraction, and multiplication. The program allows the user to input polynomial operations in a specific format and then performs the requested operation by using two programs , the first is the consumer "producer" and the second is the customer , the two programs are using a shared memory in bounded buffer way and for solving the synchronizing in the program we use mutex and semaphore , and we use threads to solve the operations in the polynom expressions
-----------------------------------

Requirements from the casing will be as follows:
1. Input of a polynoms from the user
2. make the demand operation
3. print the finall result
4. The program will exit when the word "END" is typed.

==functions==
one main function:
* main function in every program

other functions:
1-printPolynomial
2-multiplyPolynomials
3-subtractPolynomials
4-addPolynomials
5-parsePolynomial

==programe files:==
the first one: polynomCalc1.c
the second one polynomCalc2a.c
the third one: polynomCalc2b.c
the fourth one polynomCalc3a.c
the fifth one: polynomCalc3b.c

==How to compile?==
make

==Input:==
"(polynom1)ADD(polynom2)" => (2:3,2,1)ADD(3:2,4,0,-1) 
pay attintion (without quatations) and at the same order , we can use ADD , SUB , MUL

==output:==
the result of the polynom calculation

example of program running:
student@student-virtual-machine:~/CLionProjects/polynomCalc$ gcc main.c -o main
student@student-virtual-machine:~/CLionProjects/polynomCalc$ ./main
(2:3,2,1)ADD(3:2,4,0,-1)
2x^3  + 7x^2  + 2x
(2:3,2,1)SUB(3:2,4,0,-1)
- 2x^3  - x^2  + 2x + 2
(2:3,2,1)MUL(3:2,4,0,-1)
6x^5  + 16x^4  + 10x^3  + x^2  - 2x - 1
END
Process finished with exit code 0

student@student-virtual-machine:~/CLionProjects/polynomCalc$ 

