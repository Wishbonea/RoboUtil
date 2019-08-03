#include "PathLoader.h"

#include "Waypoint.h"

vector<Path> PathLoader::loadPaths(string text) {
    json loadedJson;

    vector<Path> loadedPaths;

    try {
        loadedJson = json::parse(text);
    } catch (const exception& e) {
        Logger::logError("Could not parse paths file:" + string(e.what()));
    }

//    Logger::logInfo("JSON path file contents:\n" + loadedJson.dump(4));
    try {
        json pathJson;
        for (auto path : loadedJson["paths"]) {
            string name = path["name"];
            Path newPath(name);
            for (auto point : path["points"]) {
                Point_OLD point1 = Point_OLD(point["x"], point["y"]);
                double speed = point["speed"];
                Waypoint waypoint(point1, speed); //TODO: Implement state command logic
                newPath.addWaypoint(waypoint);
            }
            loadedPaths.push_back(newPath);
        }
    } catch (const exception& e) {
        Logger::logError("Error reading json path! " + string(e.what()));
    }

    return loadedPaths;
}
