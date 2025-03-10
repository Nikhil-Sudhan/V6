#include "../../include/simulation/SimulationView.h"
#include <QVector3D>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>

SimulationView::SimulationView(QWidget* parent) : QWidget(parent)
{
    setupUI();
    createSimulationHtml();
    startPathUpdateTimer();
}

void SimulationView::setupUI()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    
    webView = new QWebEngineView(this);
    layout->addWidget(webView);
}

void SimulationView::loadSimulation()
{
    webView->setHtml(simulationHtml, QUrl("qrc:/"));
    updateDronePath("Atlas"); // Initial path load
}

void SimulationView::startPathUpdateTimer()
{
    // Create update timer for path
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        updateDronePath("Atlas");
    });
    timer->start(1000); // Update every second
}

void SimulationView::updateDronePath(const QString& droneName)
{
    // Get the absolute path to the build directory
    QDir buildDir(QCoreApplication::applicationDirPath());
    buildDir.cdUp(); // Move up from the executable location to build directory
    
    // Construct the full path to the GeoJSON file
    QString filename = buildDir.absoluteFilePath(QString("drone_geojson/%1_path.geojson").arg(droneName));
    QFile file(filename);
    
    qDebug() << "Trying to open file:" << filename;
    
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();
        
        // Convert to string and escape properly for JS
        QString jsonString = QString::fromUtf8(data).replace("\\", "\\\\").replace("'", "\\'").replace("\n", "\\n");
        
        // Update path in JavaScript
        QString script = QString("updateDronePathFromData('%1');").arg(jsonString);
        webView->page()->runJavaScript(script);
        
        // Log success
        qDebug() << "Successfully loaded and updated path for drone:" << droneName;
    } else {
        qDebug() << "Failed to open file:" << filename;
        qDebug() << "Current working directory:" << QDir::currentPath();
        qDebug() << "Application directory:" << QCoreApplication::applicationDirPath();
    }
}

void SimulationView::setDronePositions(const QVector<QVector3D>& positions)
{
    QJsonArray droneArray;
    
    for (const QVector3D& pos : positions) {
        QJsonObject drone;
        drone["x"] = pos.x();
        drone["y"] = pos.y();
        drone["z"] = pos.z();
        droneArray.append(drone);
    }
    
    QJsonDocument doc(droneArray);
    QString jsonString = doc.toJson(QJsonDocument::Compact);
    
    QString script = QString("updateDronePositions(%1);").arg(jsonString);
    webView->page()->runJavaScript(script);
}

void SimulationView::createSimulationHtml()
{
    simulationHtml = R"(
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>Drone Simulation</title>
    <style>
        body { margin: 0; overflow: hidden; background-color: #000; }
        canvas { display: block; }
        #info {
            position: absolute;
            top: 10px;
            left: 10px;
            color: white;
            font-family: Arial, sans-serif;
            font-size: 14px;
            background-color: rgba(0,0,0,0.5);
            padding: 10px;
            border-radius: 5px;
        }
        #controls {
            position: absolute;
            bottom: 10px;
            left: 10px;
            color: white;
            font-family: Arial, sans-serif;
            font-size: 12px;
            background-color: rgba(0,0,0,0.5);
            padding: 10px;
            border-radius: 5px;
        }
    </style>
</head>
<body>
    <div id="info">Drone Simulation - Testing Environment</div>
    <div id="controls">Camera Controls: Left-click + drag to rotate | Right-click + drag to pan | Scroll to zoom</div>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/three.js/r128/three.min.js"></script>
    <script src="https://cdn.jsdelivr.net/npm/three@0.128.0/examples/js/controls/OrbitControls.js"></script>
    <script>
        // Scene setup
        const scene = new THREE.Scene();
        scene.background = new THREE.Color(0x87ceeb); // Sky blue background
        
        // Camera setup
        const camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);
        camera.position.set(0, 30, 50);
        camera.lookAt(0, 0, 0);
        
        // Renderer setup
        const renderer = new THREE.WebGLRenderer({ antialias: true });
        renderer.setSize(window.innerWidth, window.innerHeight);
        renderer.shadowMap.enabled = true;
        renderer.shadowMap.type = THREE.PCFSoftShadowMap;
        document.body.appendChild(renderer.domElement);
        
        // Add orbit controls
        const controls = new THREE.OrbitControls(camera, renderer.domElement);
        controls.enableDamping = true;
        controls.dampingFactor = 0.05;
        
        // Lights
        const ambientLight = new THREE.AmbientLight(0x404040, 0.5);
        scene.add(ambientLight);
        
        const directionalLight = new THREE.DirectionalLight(0xffffff, 1);
        directionalLight.position.set(50, 200, 100);
        directionalLight.castShadow = true;
        directionalLight.shadow.mapSize.width = 2048;
        directionalLight.shadow.mapSize.height = 2048;
        directionalLight.shadow.camera.left = -100;
        directionalLight.shadow.camera.right = 100;
        directionalLight.shadow.camera.top = 100;
        directionalLight.shadow.camera.bottom = -100;
        scene.add(directionalLight);
        
        // Add hemisphere light for better outdoor lighting
        const hemisphereLight = new THREE.HemisphereLight(0xffffbb, 0x080820, 0.5);
        scene.add(hemisphereLight);
        
        // Ground plane with texture
        const textureLoader = new THREE.TextureLoader();
        const groundTexture = textureLoader.load('https://threejs.org/examples/textures/terrain/grasslight-big.jpg');
        groundTexture.wrapS = THREE.RepeatWrapping;
        groundTexture.wrapT = THREE.RepeatWrapping;
        groundTexture.repeat.set(25, 25);
        
        const groundGeometry = new THREE.PlaneGeometry(500, 500, 32, 32);
        const groundMaterial = new THREE.MeshStandardMaterial({ 
            map: groundTexture,
            side: THREE.DoubleSide,
            roughness: 0.8
        });
        const ground = new THREE.Mesh(groundGeometry, groundMaterial);
        ground.rotation.x = -Math.PI / 2;
        ground.receiveShadow = true;
        scene.add(ground);
        
        // Add terrain features - hills
        function createHill(x, z, radius, height) {
            const hillGeometry = new THREE.ConeGeometry(radius, height, 32);
            const hillMaterial = new THREE.MeshStandardMaterial({ 
                color: 0x3a7d3a,
                roughness: 0.9
            });
            const hill = new THREE.Mesh(hillGeometry, hillMaterial);
            hill.position.set(x, height/2, z);
            hill.castShadow = true;
            hill.receiveShadow = true;
            scene.add(hill);
        }
        
        // Create some hills
        createHill(-80, -60, 40, 30);
        createHill(100, 40, 50, 40);
        createHill(20, -100, 30, 20);
        
        // Add some trees
        function createTree(x, z) {
            const treeGroup = new THREE.Group();
            
            // Tree trunk
            const trunkGeometry = new THREE.CylinderGeometry(0.5, 0.8, 5, 8);
            const trunkMaterial = new THREE.MeshStandardMaterial({ color: 0x8B4513 });
            const trunk = new THREE.Mesh(trunkGeometry, trunkMaterial);
            trunk.position.y = 2.5;
            trunk.castShadow = true;
            treeGroup.add(trunk);
            
            // Tree foliage
            const foliageGeometry = new THREE.ConeGeometry(3, 8, 8);
            const foliageMaterial = new THREE.MeshStandardMaterial({ color: 0x2E8B57 });
            const foliage = new THREE.Mesh(foliageGeometry, foliageMaterial);
            foliage.position.y = 9;
            foliage.castShadow = true;
            treeGroup.add(foliage);
            
            treeGroup.position.set(x, 0, z);
            scene.add(treeGroup);
        }
        
        // Add some trees around the scene
        for (let i = 0; i < 30; i++) {
            const x = Math.random() * 400 - 200;
            const z = Math.random() * 400 - 200;
            // Don't place trees too close to the center
            if (Math.sqrt(x*x + z*z) > 50) {
                createTree(x, z);
            }
        }
        
        // Create podiums for drones
        function createPodium(x, z) {
            const podiumGeometry = new THREE.CylinderGeometry(3, 3, 1, 32);
            const podiumMaterial = new THREE.MeshStandardMaterial({ 
                color: 0x808080,
                roughness: 0.5,
                metalness: 0.7
            });
            const podium = new THREE.Mesh(podiumGeometry, podiumMaterial);
            podium.position.set(x, 0.5, z);
            podium.castShadow = true;
            podium.receiveShadow = true;
            scene.add(podium);
            return podium;
        }
        
        // Create three podiums
        const podium1 = createPodium(-20, 0);
        const podium2 = createPodium(0, 0);
        const podium3 = createPodium(20, 0);
        
        // Drone model
        function createDrone() {
            const droneGroup = new THREE.Group();
            
            // Drone body
            const bodyGeometry = new THREE.BoxGeometry(3, 0.5, 3);
            const bodyMaterial = new THREE.MeshStandardMaterial({ 
                color: 0x333333,
                metalness: 0.8,
                roughness: 0.2
            });
            const body = new THREE.Mesh(bodyGeometry, bodyMaterial);
            body.castShadow = true;
            droneGroup.add(body);
            
            // Drone arms
            const armGeometry = new THREE.BoxGeometry(1, 0.2, 0.2);
            const armMaterial = new THREE.MeshStandardMaterial({ 
                color: 0x555555,
                metalness: 0.5
            });
            
            // Four arms
            const arm1 = new THREE.Mesh(armGeometry, armMaterial);
            arm1.position.set(2, 0, 0);
            arm1.castShadow = true;
            droneGroup.add(arm1);
            
            const arm2 = new THREE.Mesh(armGeometry, armMaterial);
            arm2.position.set(-2, 0, 0);
            arm2.castShadow = true;
            droneGroup.add(arm2);
            
            const arm3 = new THREE.Mesh(armGeometry, armMaterial);
            arm3.position.set(0, 0, 2);
            arm3.rotation.y = Math.PI / 2;
            arm3.castShadow = true;
            droneGroup.add(arm3);
            
            const arm4 = new THREE.Mesh(armGeometry, armMaterial);
            arm4.position.set(0, 0, -2);
            arm4.rotation.y = Math.PI / 2;
            arm4.castShadow = true;
            droneGroup.add(arm4);
            
            // Propellers
            const propGeometry = new THREE.CylinderGeometry(0.8, 0.8, 0.1, 16);
            const propMaterial = new THREE.MeshStandardMaterial({ 
                color: 0x00a6ff,
                metalness: 0.3,
                roughness: 0.7
            });
            
            const prop1 = new THREE.Mesh(propGeometry, propMaterial);
            prop1.position.set(2, 0.2, 0);
            prop1.castShadow = true;
            droneGroup.add(prop1);
            
            const prop2 = new THREE.Mesh(propGeometry, propMaterial);
            prop2.position.set(-2, 0.2, 0);
            prop2.castShadow = true;
            droneGroup.add(prop2);
            
            const prop3 = new THREE.Mesh(propGeometry, propMaterial);
            prop3.position.set(0, 0.2, 2);
            prop3.castShadow = true;
            droneGroup.add(prop3);
            
            const prop4 = new THREE.Mesh(propGeometry, propMaterial);
            prop4.position.set(0, 0.2, -2);
            prop4.castShadow = true;
            droneGroup.add(prop4);
            
            // Add camera to drone
            const cameraGeometry = new THREE.BoxGeometry(0.5, 0.5, 0.5);
            const cameraMaterial = new THREE.MeshStandardMaterial({ color: 0x000000 });
            const cameraObj = new THREE.Mesh(cameraGeometry, cameraMaterial);
            cameraObj.position.set(0, -0.3, 1.2);
            droneGroup.add(cameraObj);
            
            // Add lights to drone
            const light1 = new THREE.PointLight(0xff0000, 0.5, 5);
            light1.position.set(2, 0, 2);
            droneGroup.add(light1);
            
            const light2 = new THREE.PointLight(0x00ff00, 0.5, 5);
            light2.position.set(-2, 0, -2);
            droneGroup.add(light2);
            
            return droneGroup;
        }
        
        // Create drones
        const drones = [];
        for (let i = 0; i < 3; i++) {
            const drone = createDrone();
            drone.position.set([-20, 0, 20][i], 2, 0);
            scene.add(drone);
            drones.push(drone);
        }
        
        // Position drones on podiums
        drones[0].position.set(-20, 1.5, 0);
        drones[1].position.set(0, 1.5, 0);
        drones[2].position.set(20, 1.5, 0);
        
        // Store drone paths
        const dronePaths = {};
        
        // Function to update drone path from data
        function updateDronePathFromData(jsonString) {
            try {
                console.log('Received path data:', jsonString);
                const data = JSON.parse(jsonString);
                
                data.features.forEach(feature => {
                    if (feature.properties.featureType === 'animation') {
                        const coordinates = feature.geometry.coordinates;
                        const droneName = feature.properties.droneName;
                        
                        console.log('Processing path for drone:', droneName);
                        console.log('Path coordinates:', coordinates);
                        
                        // Create path geometry with thicker line
                        const points = coordinates.map(coord => {
                            // Scale up the height for better visibility
                            return new THREE.Vector3(coord[0], coord[2], coord[1]);
                        });
                        
                        if (dronePaths[droneName]) {
                            // Update existing path
                            console.log('Updating existing path');
                            const pathGeometry = new THREE.BufferGeometry().setFromPoints(points);
                            dronePaths[droneName].geometry.dispose();
                            dronePaths[droneName].geometry = pathGeometry;
                        } else {
                            // Create new path with thicker, more visible line
                            console.log('Creating new path');
                            const pathGeometry = new THREE.BufferGeometry().setFromPoints(points);
                            const pathMaterial = new THREE.LineBasicMaterial({ 
                                color: 0xff0000,
                                linewidth: 5,
                                transparent: true,
                                opacity: 0.8
                            });
                            
                            const pathLine = new THREE.Line(pathGeometry, pathMaterial);
                            scene.add(pathLine);
                            dronePaths[droneName] = pathLine;
                            
                            // Add debug sphere at start and end points
                            const sphereGeometry = new THREE.SphereGeometry(0.5, 32, 32);
                            const sphereMaterial = new THREE.MeshBasicMaterial({ color: 0xffff00 });
                            
                            const startSphere = new THREE.Mesh(sphereGeometry, sphereMaterial);
                            startSphere.position.copy(points[0]);
                            scene.add(startSphere);
                            
                            const endSphere = new THREE.Mesh(sphereGeometry, sphereMaterial);
                            endSphere.position.copy(points[points.length - 1]);
                            scene.add(endSphere);
                        }
                    }
                });
            } catch (error) {
                console.error('Error updating drone path:', error);
                console.error('JSON string was:', jsonString);
            }
        }
        
        // Animation loop
        let propRotation = 0;
        function animate() {
            requestAnimationFrame(animate);
            
            // Update controls
            controls.update();
            
            // Rotate propellers
            propRotation += 0.2;
            drones.forEach(drone => {
                drone.children.slice(5, 9).forEach(prop => {
                    prop.rotation.y = propRotation;
                });
            });
            
            renderer.render(scene, camera);
        }
        
        // Handle window resize
        window.addEventListener('resize', () => {
            camera.aspect = window.innerWidth / window.innerHeight;
            camera.updateProjectionMatrix();
            renderer.setSize(window.innerWidth, window.innerHeight);
        });
        
        // Start animation
        animate();
    </script>
</body>
</html>
    )";
} 