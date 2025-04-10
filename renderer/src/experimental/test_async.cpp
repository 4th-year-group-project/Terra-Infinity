#include <vector>
#include <memory>
#include <mutex>
#include <future>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <curl/curl.h> // This will be used to complete the http requests

#include <nlohmann/json.hpp> // This will be used to parse the json data


int main(int argc, char** argv);

struct PacketData {
    std::vector<char> rawData;
    long seed;
    int cx;
    int cz;
    int num_vertices;
    int vx;
    int vz;
    int size;
    int lenHeightmapData;
    int biomeDataSize;
    int lenBiomeData;
    std::vector<std::vector<float>> heightmapData;
    std::vector<std::vector<uint8_t>> biomeData;
};

int parsePacketData(char* data, size_t size, PacketData* buffer) {
    // Using pointer math to parse the data byte by byte
    int index = 0;
    buffer->seed = *reinterpret_cast<long*>(data + index);
    index += sizeof(long);
    buffer->cx = *reinterpret_cast<int*>(data + index);
    index += sizeof(int);
    buffer->cz = *reinterpret_cast<int*>(data + index);
    index += sizeof(int);
    buffer->num_vertices = *reinterpret_cast<int*>(data + index);
    index += sizeof(int);
    buffer->vx = *reinterpret_cast<int*>(data + index);
    index += sizeof(int);
    buffer->vz = *reinterpret_cast<int*>(data + index);
    index += sizeof(int);
    buffer->size = *reinterpret_cast<int*>(data + index);
    index += sizeof(int);
    buffer->lenHeightmapData = *reinterpret_cast<uint32_t*>(data + index);
    index += sizeof(uint32_t);
    buffer->biomeDataSize = *reinterpret_cast<int*>(data + index);
    index += sizeof(int);
    buffer->lenBiomeData = *reinterpret_cast<uint32_t*>(data + index);
    index += sizeof(uint32_t);
    // Ensure that the length of the heightmap data is correct
    if (buffer->lenHeightmapData != buffer->num_vertices * (buffer->size / 8)) {
        std::cerr << "ERROR: The length of the heightmap data does not match the expected length" << std::endl;
        return 1;
    }
    // Extract the heightmap data
    for (int z = 0; z < buffer->vz; z++) {
        std::vector<float> heightmapRow;
        for (int x = 0; x < buffer->vx; x++) {
            // We know that each element in the heightmap data is size bits long (16 bits)
            uint16_t entry = *reinterpret_cast<uint16_t*>(data + index);
            index += sizeof(uint16_t);
            // We need to ensure that the value ranges from 0 to 1
            float entryFloat = static_cast<float>(entry) / 65535.0f;
            heightmapRow.push_back(entryFloat);
        }
        buffer->heightmapData.push_back(heightmapRow);
    }
    // Extract the biome data
    for (int z = 0; z < buffer->vz; z++) {
        std::vector<uint8_t> biomeRow;
        for (int x = 0; x < buffer->vx; x++) {
            // We know that each element in the biome data is 8 bits long
            uint8_t entry = *reinterpret_cast<uint8_t*>(data + index);
            index += sizeof(uint8_t);
            // We need to ensure that the value ranges from 0 to 1
            biomeRow.push_back(entry);
        }
        buffer->biomeData.push_back(biomeRow);
    }
    std::cout << "Index: " << index << std::endl;
    if (static_cast<size_t>(index) != size) {
        std::cerr << "ERROR: The index does not match the total size of the data" << std::endl;
        return 1;
    }
    return 0;
}

class Chunk {
private:
    int cx;
    int cz;
    std::vector<std::vector<float>> heights;
    // We need to store the biome data as well
    std::vector<std::vector<uint8_t>> biomeData;

public:
    Chunk(int cx, int cz, std::vector<std::vector<float>> heights, std::vector<std::vector<uint8_t>> biomeData) :
        cx(cx), cz(cz), heights(heights), biomeData(biomeData) {}

    Chunk(int cx, int cz) : cx(cx), cz(cz) {
        // Initialize heights with some default values
        heights = std::vector<std::vector<float>>(1026, std::vector<float>(1026, 0.0f));
        biomeData = std::vector<std::vector<uint8_t>>(1026, std::vector<uint8_t>(1026, 0));
    }

    int getCX() const { return cx; }
    int getCZ() const { return cz; }
    std::vector<std::vector<float>> getHeights() const { return heights; }
    void setHeights(const std::vector<std::vector<float>>& newHeights) {
        heights = newHeights;
    }
    void setBiomeData(const std::vector<std::vector<uint8_t>>& newBiomeData) {
        biomeData = newBiomeData;
    }
    std::vector<std::vector<uint8_t>> getBiomeData() const { return biomeData; }

    void render() const {
        // Render the chunk using OpenGL or any other rendering library
        // For demonstration, we'll just print the chunk coordinates
        std::cout << "(" << cx << ", " << cz << "),";
    }
};



size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    auto *buffer = static_cast<PacketData*>(userp);
    char *data = static_cast<char*>(contents);
    // We are now going to accumulate the data into the buffer
    size_t oldSize = buffer->rawData.size();
    buffer->rawData.resize(oldSize + totalSize);
    std::memcpy(buffer->rawData.data() + oldSize, data, totalSize);
    return totalSize;
}

PacketData requestNewChunk(int cx, int cz, long seed) {
    // Create the JSON object for the additional parameters
    nlohmann::json payload = {
        {"mock_data", false},
        {"debug", false},
        {"seed", seed},
        {"cx", cx},
        {"cy", cz},
        {"biome", nullptr},
        {"debug", true},
        {"biome_size", 50},
        {"ocean_coverage", 50},
        {"land_water_scale", 50},
        {"global_max_height", 100},
        {"temperate_rainforest", {
            {"max_height", 30}}},
        {"boreal_forest", {
            {"max_height", 40}}},
        {"grassland", {
            {"max_height", 40}}},
        {"tundra", {
            {"max_height", 50}}},
        {"savanna", {
            {"max_height", 25}}},
        {"woodland", {
            {"max_height", 40}}},
        {"tropical_rainforest", {
            {"max_height", 35}}},
        {"temperate_seasonal_forest", {
            {"max_height", 100}}},
        {"subtropical_desert", {
            {"max_height", 30}}},
    };

    // Print the JSON payload pretty
    // std::cout << "JSON payload: " << payload.dump(4) << std::endl;
    CURL* curl;
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) {
        std::cerr << "ERROR: Failed to initialize curl" << std::endl;
        return PacketData();
    }
    curl = curl_easy_init();
    if (!curl) {
        std::cerr << "ERROR: Failed to initialize curl" << std::endl;
        return PacketData();
    }
    struct curl_slist* headers = nullptr;
    // Update header to explicitly specify UTF-8 encoding
    headers = curl_slist_append(headers, "Content-Type: application/json; charset=utf-8");

    //Setting the curl options
    curl_easy_setopt(curl, CURLoption::CURLOPT_URL, "http://localhost:8000/superchunk");
    curl_easy_setopt(curl, CURLoption::CURLOPT_POST, 1L);
    // Ensure the JSON payload is properly UTF-8 encoded
    std::string jsonPayload = payload.dump();
    curl_easy_setopt(curl, CURLoption::CURLOPT_POSTFIELDS, jsonPayload.c_str());
    curl_easy_setopt(curl, CURLoption::CURLOPT_POSTFIELDSIZE, payload.dump().size());
    // Adding a timeout to the request
    curl_easy_setopt(curl, CURLoption::CURLOPT_TIMEOUT, 30L);
    // Modifying the buffer to be a 50MB buffer
    curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 1024 * 1024 * 50L); // 50MB buffer
    curl_easy_setopt(curl, CURLoption::CURLOPT_HTTPHEADER, headers);
    // curl_easy_setopt(curl, CURLoption::CURLOPT_VERBOSE, 1L);

    // Setting the write callback function
    PacketData packetData;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &packetData);

    // Perform the request
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "ERROR: Failed to perform curl request: " << curl_easy_strerror(res) << std::endl;
    } else {
        // Parse the response data
        parsePacketData(packetData.rawData.data(), packetData.rawData.size(), &packetData);
        std::cout << "Chunk data received successfully" << std::endl;
    }

    // We are now going to output some of the data to the console to verify that it is correct
    std::cout << "=========================== PACKET DATA ===========================" << std::endl;
    std::cout << "Seed: " << packetData.seed << std::endl;
    std::cout << "cx: " << packetData.cx << std::endl;
    std::cout << "cz: " << packetData.cz << std::endl;
    std::cout << "num_vertices: " << packetData.num_vertices << std::endl;
    std::cout << "vx: " << packetData.vx << std::endl;
    std::cout << "vz: " << packetData.vz << std::endl;
    std::cout << "size: " << packetData.size << std::endl;
    std::cout << "lenHeightmapData: " << packetData.lenHeightmapData << std::endl;
    std::cout << "biomeDataSize: " << packetData.biomeDataSize << std::endl;
    std::cout << "lenBiomeData: " << packetData.lenBiomeData << std::endl;
    std::cout << "===================================================================" << std::endl;
    // Clean up
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return packetData;
}

class ChunkManager {
private:
    std::vector<std::shared_ptr<Chunk>> chunks;
    std::mutex chunkMutex;
    std::mutex requestMutex;
    // We need a list of requests that are currently being processed
    std::vector<std::pair<int, int>> requests;

public:
    void addChunk(std::shared_ptr<Chunk> chunk) {
        std::lock_guard<std::mutex> lock(chunkMutex);
        chunks.push_back(chunk);
    }

    void removeChunk(int cx, int cz) {
        std::lock_guard<std::mutex> lock(chunkMutex);
        chunks.erase(std::remove_if(chunks.begin(), chunks.end(),
            [cx, cz](const std::shared_ptr<Chunk>& chunk) {
                return chunk->getCX() == cx && chunk->getCZ() == cz;
            }), chunks.end());
    }

    std::shared_ptr<Chunk> getChunk(int cx, int cz) {
        std::lock_guard<std::mutex> lock(chunkMutex);
        for (const auto& chunk : chunks) {
            if (chunk->getCX() == cx && chunk->getCZ() == cz) {
                return chunk;
            }
        }
        return nullptr;
    }

    void renderChunks() {
        std::lock_guard<std::mutex> lock(chunkMutex);
        std::cout << "Rendering chunks: ";
        for (const auto& chunk : chunks) {
            chunk->render();
        }
        std::cout << "\n";
    }

    void clearChunks() {
        std::lock_guard<std::mutex> lock(chunkMutex);
        chunks.clear();
    }

    size_t getChunkCount() {
        std::lock_guard<std::mutex> lock(chunkMutex);
        return chunks.size();
    }

    // We need to check if the chunk is already being requested
    bool isChunkRequested(int cx, int cz) {
        std::lock_guard<std::mutex> lock(requestMutex);
        for (const auto& request : requests) {
            if (request.first == cx && request.second == cz) {
                return true;
            }
        }
        return false;
    }
    // We need to add the chunk to the list of requests
    void addChunkRequest(int cx, int cz) {
        std::lock_guard<std::mutex> lock(requestMutex);
        requests.push_back(std::make_pair(cx, cz));
    }
    // We need to remove the chunk from the list of requests
    void removeChunkRequest(int cx, int cz) {
        std::lock_guard<std::mutex> lock(requestMutex);
        requests.erase(std::remove_if(requests.begin(), requests.end(),
            [cx, cz](const std::pair<int, int>& request) {
                return request.first == cx && request.second == cz;
            }), requests.end());
    }
    // We need to check if the chunk is already being requested

    void printRequests() {
        std::lock_guard<std::mutex> lock(requestMutex);
        std::cout << "Current requests: ";
        for (const auto& request : requests) {
            std::cout << "(" << request.first << ", " << request.second << "), ";
        }
        std::cout << "\n";
    }

    int requestInitialChunks(long seed, std::vector<std::pair<int, int>> initialChunks) {
        // Launch the initial chunk requests asynchronously
        std::vector<std::future<PacketData>> futures;
        for (const auto& [cx, cz] : initialChunks) {
            addChunkRequest(cx, cz);
            std::cout << "Requesting initial chunk at (" << cx << ", " << cz << ")" << std::endl;
            futures.push_back(std::async(std::launch::async, requestNewChunk, cx, cz, seed));
        }
        std::vector<std::pair<int, int>> failedRequests;
        std::vector<std::future<PacketData>> RerequestFutures;
        // Wait for all of the requests to complete
        for (size_t i = 0; i < futures.size(); ++i) {
            PacketData packetData = futures[i].get(); // Blocks until the request is complete
            // Check if the request was successful
            if (packetData.rawData.empty()) {
                std::cerr << "ERROR: Failed to request initial chunk at (" << initialChunks[i].first << ", " << initialChunks[i].second << ")" << std::endl;
                failedRequests.push_back(initialChunks[i]);
                continue;
            }
            // Create the new chunk
            std::shared_ptr<Chunk> newChunk = std::make_shared<Chunk>(packetData.cx, packetData.cz, packetData.heightmapData, packetData.biomeData);
            // Add the chunk to the chunk manager
            addChunk(newChunk);
            // Remove the chunk from the list of requests
            removeChunkRequest(packetData.cx, packetData.cz);
            std::cout << "Initial chunk at (" << packetData.cx << ", " << packetData.cz << ") has been added" << std::endl;
        }
        // We will try to request the failed requests again
        for (const auto& [cx, cz] : failedRequests) {
            std::cout << "Retrying initial chunk request at (" << cx << ", " << cz << ")" << std::endl;
            addChunkRequest(cx, cz);
            RerequestFutures.push_back(std::async(std::launch::async, requestNewChunk, cx, cz, seed));
        }
        // Wait for all of the rerequests to complete
        for (size_t i = 0; i < RerequestFutures.size(); ++i) {
            PacketData packetData = RerequestFutures[i].get(); // Blocks until the request is complete
            // Check if the request was successful
            if (packetData.rawData.empty()) {
                std::cerr << "ERROR: Failed to request initial chunk at (" << failedRequests[i].first << ", " << failedRequests[i].second << ")" << std::endl;
                continue;
            }
            // Create the new chunk
            std::shared_ptr<Chunk> newChunk = std::make_shared<Chunk>(packetData.cx, packetData.cz, packetData.heightmapData, packetData.biomeData);
            // Add the chunk to the chunk manager
            addChunk(newChunk);
            // Remove the chunk from the list of requests
            removeChunkRequest(packetData.cx, packetData.cz);
            std::cout << "Initial chunk at (" << packetData.cx << ", " << packetData.cz << ") has finally been added" << std::endl;
        }
        std::cout << "All initial chunks have been requested" << std::endl;
        return 0;
    }

    int regenerateInitialChunks(long seed, std::pair<float, float> position) {
        // This function will be called to regenerate the initial chunks
        // We will first clear the chunks
        clearChunks();
        // We will then request a new block of 4x4 chunks which will depend on the position of the player
        int chunkX = static_cast<int>(floor(position.first / 1024));
        int chunkZ = static_cast<int>(floor(position.second / 1024));
        // Now depending on the quadrant of the chunk the camera is in will determine the 4 chunks that are requested
        float xOffset = fmod(position.first, 1024);
        float zOffset = fmod(position.second, 1024);
        std::vector<std::pair<int, int>> initialChunks;
        if (xOffset < 512 && zOffset < 512) {
            // Top left quadrant
            initialChunks = {
                {chunkX - 1, chunkZ - 1},
                {chunkX - 1, chunkZ},
                {chunkX, chunkZ - 1},
                {chunkX, chunkZ}
            };
        } else if (xOffset >= 512 && zOffset < 512) {
            // Top right quadrant
            initialChunks = {
                {chunkX, chunkZ - 1},
                {chunkX + 1, chunkZ - 1},
                {chunkX, chunkZ},
                {chunkX + 1, chunkZ}
            };
        } else if (xOffset < 512 && zOffset >= 512) {
            // Bottom left quadrant
            initialChunks = {
                {chunkX - 1, chunkZ},
                {chunkX - 1, chunkZ + 1},
                {chunkX, chunkZ},
                {chunkX, chunkZ + 1}
            };
        } else {
            // Bottom right quadrant
            initialChunks = {
                {chunkX, chunkZ},
                {chunkX + 1, chunkZ},
                {chunkX, chunkZ + 1},
                {chunkX + 1, chunkZ + 1}
            };
        }
        // Now we will request the initial chunks
        requestInitialChunks(seed, initialChunks);
        return 0;
    }

    int requestNewChunkAsync(int cx, int cz, long seed) {
        // This function will be called asynchronously to request a new chunk
        // Check to see if the chunk is already being requested
        if (isChunkRequested(cx, cz) && getChunk(cx, cz) != nullptr) {
            std::cout << "Chunk at (" << cx << ", " << cz << ") is already being requested or exists" << std::endl;
            return 1;
        } else {
            // Add the chunk to the list of requests
            addChunkRequest(cx, cz);
            std::cout << "Requesting chunk at (" << cx << ", " << cz << ")" << std::endl;
            std::future<PacketData> futureData = std::async(std::launch::async, requestNewChunk, cx, cz, seed);

            std::thread([this, future=std::move(futureData), cx, cz]() mutable {
                // Wait for the request to complete
                PacketData packetData = future.get();
                // Check that the request was successful
                if (packetData.rawData.empty()) {
                    std::cerr << "ERROR: Failed to request chunk at (" << cx << ", " << cz << ")" << std::endl;
                    return;
                }
                // Create the new chunk
                std::shared_ptr<Chunk> newChunk = std::make_shared<Chunk>(cx, cz, packetData.heightmapData, packetData.biomeData);
                // Add the chunk to the chunk manager
                addChunk(newChunk);
                // Remove the chunk from the list of requests
                removeChunkRequest(cx, cz);
                std::cout << "Chunk at (" << cx << ", " << cz << ") has been added" << std::endl;
            }).detach();
        }
        return 0;
    }
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
int main(int argc, char** argv){
    // This is a test for the async functionality of the renderer
    std::cout << "Hello, World!" << std::endl;
    std::cout << "This is a test for the async functionality of the renderer" << std::endl;

    ChunkManager chunkManager;

    chunkManager.regenerateInitialChunks(23, std::make_pair(0.0f, 0.0f));

    chunkManager.renderChunks();
    chunkManager.regenerateInitialChunks(23, std::make_pair(5433234.0f, -24575132.0f));

    chunkManager.renderChunks();

    // chunkManager.requestNewChunkAsync(0, 0, 23);
    // chunkManager.requestNewChunkAsync(0, 0, 23);
    // chunkManager.requestNewChunkAsync(-1, 0, 23);
    // chunkManager.requestNewChunkAsync(0, -1, 23);
    // chunkManager.requestNewChunkAsync(-1, -1, 23);

    // chunkManager.printRequests();

    // while (true) {
    //     // Render the chunks
    //     chunkManager.renderChunks();
    //     chunkManager.printRequests();
    //     // Sleep for a while to simulate rendering
    //     std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // }
}
#pragma GCC diagnostic pop