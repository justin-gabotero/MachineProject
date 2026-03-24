#include "donation_system.h"
#include "user_input.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// Function to prompt the user for donation details and create a Donation record
//@param donor, the User struct representing the donor making the donation
//@param outDonation, a pointer to a Donation struct where the created donation record will be stored 
int addDonationPrompt(User donor, Donation *outDonation){

    Donation temp;
    char buf[128];
    int status = 0;
    // Copy the donors username into the temp variable
    strcpy(temp.donor, donor.username);
    //Ask the user for the type of food they want to donate
    printf("Food type: ");
    status = readLine(temp.foodType, sizeof(temp.foodType));
    if(status != 0 || strlen(temp.foodType) == 0){
        return -1
    }
    //Ask the user for the quantity of food they want to donate
    printf("Quantity: ");
    //converts string input to an integer and checks if it is a valid positive number
    status = readLine(buf, sizeof(buf));
    if(status != 0 || strlen(buf) == 0){
        return -1;
    }
    if(sscanf(buf, "%d", &temp.quantity) != 1 || temp.quantity <= 0){
        return -1;
    }
    //Ask the user for the expiration date of the food they want to donate
    printf("Expiration Date(MM-DD-YYYY): ");
    status = readLine(buf, sizeof(buf));
    //converts string input to an integer and checks if it is a valid positive number
    if(status != 0 || strlen(buf) == 0){
        return -1;
    }
    if(sscanf(buf, "%d-%d-%d", &temp.monthExpiration, &temp.dayExpiration, &temp.yearExpiration) != 3){
        return -1;
    }
    //Ask the user for the expiration date of the food they want to donate
    printf("Pickup Location; ");
    status = readLine(temp.pickupLocation, sizeof(temp.pickupLocation));
    if(status != 0 || strlen(temp.pickupLocation) == 0){
        return -1;
    }
    // If all inputs are valid, copy the temp variable to the output variable so that outDonation not contains the Donation Record created fromthe user input
    *outDonation = temp;
    return 0;
    }
// Function to create a Donation record from a given Donation struct, checks if the input values are valid and if so copies the input to the output variable
//@param in, a Donation struct containing the details of the donation to be created
//@param out, a Donation struct where the created donation record will be stored
int createDonation(Donation in, Donation *out){
    //Checks if donation inputs are all valid and have valid values, if not returns -1 to indicate an error
    if(in.quantity <= 0 || strlen(in.foodType) == 0 || strlen(in.donor) == 0 || strlen(in.pickupLocation) == 0){
        return -1;
    }
    //checking date validity, month should be between 1 and 12, day should be between 1 and 31, year should be non-negative
    if(in.yearExpiration < 0 || in.monthExpiration < 1 || in.monthExpiration > 12 || in.dayExpiration < 1 || in.dayExpiration > 31){
        return -1;
    }
    // If all inputs are valid, copy the in variable to the output variable
    *out = in;
    return 0;
}

double computeDonationWasteReduction(Donation donation){
    

}


void writeDomation(Donation donation){



}





