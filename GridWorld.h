
#ifndef _GRID_WORLD_H
#define _GRID_WORLD_H

#include <vector>
#include <iostream>

using std::vector;
using std::cout;


class GridWorld {

  private:
    // private stuff goes here!
    //   typedefs data members private helper functions etc.
    //struct for a single Person

    //struct for a doubly-linked list
    struct Node {
      int data;     //data is an ID number
      Node *next, *before;

      Node(int i, Node *n, Node *b)
        {data = i;    next = n;   before =b;}
    };
    //struct for a person
    struct Person {
      int id, row, col;
      bool alive;
      Node *pDist;
    };
    //struct for a district of the grid
    struct District {
      Node *front, *back;
      int population;
    };
    // Data members of the class
    int SIZE, ROWS, COLS, currID;
    vector<int> Deadpool;
    vector<Person> people;
    District **WORLD;

    //HELPER FUNCTIONS==============================================
    //helper function to clear a list
    void clear(District *d) {
      Node *p = d->front;
      Node *pNext;
      while(p!=nullptr) {
        pNext = p->next;
        delete p;
        p = pNext;
      }
      d->front = d->back = nullptr;
      d->population = 0;
    }

    //helper funtion to add people to specified district
    void addToDistrict(int id, int row, int col){
      //create a new node
      Node *tmp = new Node(id, nullptr, nullptr);
      //if district is empty
      if(WORLD[row][col].population == 0){
        WORLD[row][col].front = tmp;
        WORLD[row][col].back = tmp;
      }
      //if district is not empty
      else {
        tmp->before = WORLD[row][col].back;
        WORLD[row][col].back->next = tmp;
        WORLD[row][col].back = tmp;
      }
      WORLD[row][col].population = WORLD[row][col].population + 1;
    }

    //helper function to remove one person from a district
    void removeFromDistrict(int personID, int row, int col) {
     Node *tmp;
      //if there is only one item
      if(WORLD[row][col].front->next == nullptr) {
        tmp = WORLD[row][col].front;
        WORLD[row][col].front = nullptr;
        WORLD[row][col].back = nullptr;
      }
      //if removing from front
      else if(WORLD[row][col].front->data == personID) {
        tmp = WORLD[row][col].front;
        WORLD[row][col].front = WORLD[row][col].front->next;
        WORLD[row][col].front->before = nullptr;
      }
      //if removing from back
      else if(WORLD[row][col].back->data == personID) {
        tmp = WORLD[row][col].back;
        WORLD[row][col].back = WORLD[row][col].back->before;
        WORLD[row][col].back->next = nullptr;
      }
      //if removing from middle
      else{
        tmp = people.at(personID).pDist;
        people.at(personID).pDist->next->before = people.at(personID).pDist->before;
        people.at(personID).pDist->before->next = people.at(personID).pDist->next;
      }
      delete tmp;
      WORLD[row][col].population = WORLD[row][col].population - 1;
    }
    //END OF HELPER FUNCTIONS=======================================

  public:
    //constructor:  initializes a "world" with nrows and
    //   ncols (nrows*ncols districtcs) in which all districtricts are empty (a wasteland!). */
    GridWorld(unsigned nrows, unsigned ncols)   {
      ROWS = nrows;
      COLS = ncols;
      SIZE = currID = 0;
      //allocate space for array
      WORLD = new District* [nrows];
      for(int i=0; i<nrows; i++)
          WORLD[i] = new District[ncols];
      //initialize each district
      for(int i=0; i<nrows; i++) {
        for(int j=0; j<ncols; j++) {
          WORLD[i][j].population = 0;
          WORLD[i][j].front = WORLD[i][j].back = nullptr;
        }
      }
    }
    //destructor
    ~GridWorld(){
      //clear each district
      for(int i=0; i<ROWS; ++i) {
        for(int j=0; j<COLS; ++j)
          clear(&WORLD[i][j]);
      }
      //delete the rest of the world
      for(int s=0; s<ROWS; s++)
          delete WORLD[s];
      delete WORLD;
      Deadpool.clear();     //clear the Deadpool
      people.clear();       //clear the people vector
      ROWS = COLS = SIZE = currID = 0;
    }

    /*
     * function: birth
     * description:  if row/col is valid, a new person is created
     *   with an ID according to rules in handout.  New person is
     *   placed in district (row, col)
     *
     * return:  indicates success/failure
     */
    bool birth(int row, int col, int &id){
      //check to see if the given row/col are valid
      if(col>=COLS || row>=ROWS)
        return false;
      //check if there are any reusable IDs
      if(Deadpool.size() == 0) {
        id = currID;
        currID++;
        people.push_back(Person());
      }
      else {
        //pop front of Deadpool
        id = Deadpool.back();
        Deadpool.pop_back();
      }

      addToDistrict(id, row, col);
      //create a new person
      people.at(id).id = id;
      people.at(id).row = row;
      people.at(id).col = col;
      people.at(id).alive = true;
      people.at(id).pDist = WORLD[row][col].back;
      //update populations
      SIZE++;
      return true;
    }

    /*
     * function: death
     * description:  if given person is alive, person is killed and
     *   data structures updated to reflect this change.
     *
     * return:  indicates success/failure
     */
    bool death(int personID){
      //make sure person is alive
      if(people.at(personID).alive == false)
        return false;
      removeFromDistrict(personID, people.at(personID).row, people.at(personID).col);
      //if Deadpool is empty
      if(Deadpool.size() == 0) {
        Deadpool.push_back(personID);
      }
      //if Deadpool is not empty
      else {
        Deadpool.insert(Deadpool.begin(), personID);
      }

      //update population of world
      people.at(personID).alive = false;
      SIZE--;
      return true;
    }

    /*
     * function: whereis
     * description:  if given person is alive, his/her current residence
     *   is reported via reference parameters row and col.
     *
     * return:  indicates success/failure
     */
    bool whereis(int id, int &row, int &col)const{
      //check if ID represents a person
      if(id > currID)
        return false;
      //check if person is alive
      if(people.at(id).alive == false)
        return false;
      //get their row & column
      row = people.at(id).row;
      col = people.at(id).col;
      return true;
    }

    /*
     * function: move
     * description:  if given person is alive, and specified target-row
     *   and column are valid, person is moved to specified district and
     *   data structures updated accordingly.
     *
     * return:  indicates success/failure
     *
     * comment/note:  the specified person becomes the 'newest' member
     *   of target district (least seniority) --  see requirements of members().
     */
    bool move(int id, int targetRow, int targetCol){
      //check if ID represents a living person
      if(id > currID || people.at(id).alive==false)
        return false;
      //check if destination is valid
      if(targetRow>=ROWS || targetCol>=COLS)
        return false;
      removeFromDistrict(id, people.at(id).row, people.at(id).col);
      //update person's row & col
      people.at(id).row = targetRow;
      people.at(id).col = targetCol;
      addToDistrict(id, targetRow, targetCol);
      return true;
    }

    /*
     * function: members
     * description:  export the current members of a particular district
     */
    std::vector<int> * members(int row, int col)const{
      vector<int>* memb = new vector<int>;
      //make sure there is such a district
      if(row<ROWS && col<COLS) {
        Node *tmp = WORLD[row][col].front;
        for(int i=0; i<WORLD[row][col].population; i++) {
          memb->push_back(tmp->data);
          tmp = tmp->next;
        }
      }
      return memb;
    }

    /*
     * function: population
     * description:  returns the current (living) population of the world.
     */
    int population()const{
      return SIZE;
    }

    /*
     * function: population(int,int)
     * description:  returns the current (living) population of specified
     *   district.  If district does not exist, zero is returned
     */
    int population(int row, int col)const{
      //check if district is valid
      if(row>=ROWS || col>=COLS)
        return 0;
      return WORLD[row][col].population;
    }

    /*
     * function: num_rows
     * description:  returns number of rows in world
     */
    int num_rows()const {
      return ROWS;
    }

    /*
     * function: num_cols
     * description:  returns number of columns in world
     */
    int num_cols()const {
      return COLS;
    }

};

#endif
