/**
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include <iostream>
#include <list>
#include <string>
#include <cstdlib>
#include <thread>
#include <mutex>
#include <fstream>
#include <vector>
#include <utility>
#include <cstring>

class Threader{
public:
    Threader(const std::string &file, unsigned threadCount);
    void run();

private:
    unsigned m_threadCount;
    std::list<std::string> m_commandQueue;
    unsigned m_initialCommandCount;
    std::mutex m_commandQueueMutex;
    bool m_isCancelled = false;

    void workerThread();
    std::pair<std::string, bool> getNextCommand();

    void cancel();
    void cancelThread();
    void statusThread();
};

Threader::Threader(const std::string &file, unsigned threadCount) : m_threadCount(threadCount) {
    std::cout << "Reading commands list: " << file << std::endl;
    try{
        std::ifstream fin;
        fin.exceptions(std::ios::failbit | std::ios::badbit);
        fin.open(file);
        fin.exceptions(std::ios::goodbit);

        std::string line;
        while(std::getline(fin, line)){
            m_commandQueue.push_back(line);
            std::cout << line << std::endl;
        }

        m_initialCommandCount = m_commandQueue.size();
    }catch(std::fstream::failure &e){
        throw std::runtime_error("Failed to read file: " + file + ": " + e.code().message());
    }
}

void Threader::run() {
    std::vector<std::thread> threads;
    threads.reserve(m_threadCount);

    for(unsigned i=0;i<m_threadCount;++i){
        threads.emplace_back([this]{workerThread();});
    }

    //these threads will be killed by the std::thread destructor once all the tasks finish
    std::thread cancelThreadObject([this]{cancelThread();});
    std::thread statusThreadObject([this]{statusThread();});

    for(std::thread &thread : threads){
        thread.join();
    }
}

void Threader::cancel() {
    std::lock_guard<std::mutex> guard(m_commandQueueMutex);
    m_isCancelled = true;
}

void Threader::workerThread() {
    while(true){
        auto nextCommand = getNextCommand();
        if(nextCommand.second){
            std::system(nextCommand.first.c_str());
        }else{
            return;
        }
    }
}

void Threader::cancelThread() {
    while(true){
        std::string word;
        std::cin >> word;
        if(word == "cancel"){
            cancel();
            std::cout << "[Simple Parallel Processes] New new tasks will be started and the task manager process will end when the last currently running task ends." << std::endl;
        }else{
            std::cout << "[Simple Parallel Processes] Unknown command: " << word << std::endl;
        }
    }
}

void Threader::statusThread() {
    while(true){
        std::this_thread::sleep_for(std::chrono::seconds(5));
        {
            std::lock_guard<std::mutex> guard(m_commandQueueMutex);
            std::cout << "[Simple Parallel Processes] " << (m_initialCommandCount - m_commandQueue.size()) << "/" << m_initialCommandCount << " tasks completed." << std::endl;
        }
    }
}

std::pair<std::string, bool> Threader::getNextCommand()     {
    std::lock_guard<std::mutex> m_guard(m_commandQueueMutex);
    if(m_commandQueue.empty() || m_isCancelled){
        return {"", false};
    }else{
        std::string command = *(m_commandQueue.begin());
        m_commandQueue.pop_front();
        return {std::move(command), true};
    }
}

int main(int argc, char *argv[]) {
    unsigned threadCount = std::thread::hardware_concurrency();
    std::string commandListFile;

    switch(argc){
        case 3:
            try{
                threadCount = std::stoi(argv[2]);
            }catch(std::exception &e){
                std::cerr << "Invalid thread count: " << argv[2] << std::endl;
                return 01;
            }
            //no break here is intentional
        case 2:
            commandListFile = argv[1];
            break;
        default:
            std::cerr << "Usage: " << argv[0] << " </path/to/command-list> [threads]" << std::endl;
            return -1;
    }

    Threader threader(commandListFile, threadCount);
    threader.run();

    return 0;
}