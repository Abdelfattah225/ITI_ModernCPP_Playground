#include <iostream>
#include <initializer_list>
#include <string>
#include <utility>
#include <vector>
 #include <fcntl.h>
#include <unistd.h>
class FileActions
{
    private:

        int *fd ; 
        int *&fdRef;
        std::vector<std::pair<std::string,int>> actions;    
    public:
        FileActions(const std::string & path) :  fdRef(fd) 
        {
            fd = new int;
            *fd = open(path.c_str(),O_RDWR, 0666);
            if (*fd < 0) 
                {
                    delete fd; 
                     throw std::runtime_error("Failed to open file: " + path);                }
        }
        FileActions(const FileActions & ohter)
        
        :   fd(ohter.fd),
            fdRef(fd),
            actions(ohter.actions)
        {
            std::cout << "Copy constructor called. Sharing file descriptor." << std::endl;
        }


        void registerActions(std::initializer_list<std::pair<std::string,int>> x)
        {
            for(const auto action_pair : x)
            {
                actions.push_back(action_pair);
            }
            
        }
        void executeActions(){
                        for (const auto& action_pair : actions) {
                if (action_pair.first == "read") {
                    std::cout << "Simulating 'read' for " << action_pair.second << " bytes." << std::endl;
                } else if (action_pair.first == "write") {
                    std::cout << "Simulating 'write' of " << action_pair.second << " bytes." << std::endl;
                } else if (action_pair.first == "close") {
                    std::cout << "Simulating 'close' action." << std::endl;
                } else {
                    std::cout << "Unknown action: " << action_pair.first << std::endl;
                }
            }
        }     
        ~FileActions()
        {
            close(*fd);
            delete fd;
        }

};

int main() {
        FileActions obj1("/home/abdelfattah/file.h");
        obj1.registerActions({{"read", 1024}, {"write", 512}});
        obj1.executeActions();

        std::cout << "\nCreating a copy of the object...\n" << std::endl;
        FileActions obj2 = obj1; 

        obj2.executeActions();
    return 0;
    }
