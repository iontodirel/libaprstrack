import requests
import json

BATCH_SIZE = 100  # Maximum number of points per request
INV = 1.0 / 1e6;

def get_route(url, locations, costing="auto", units="miles"):
    """Fetches a route from the Valhalla API given a URL and a list of locations."""
    headers = {"Content-Type": "application/json"}
    payload = {
        "locations": [{"lat": loc[0], "lon": loc[1]} for loc in locations],
        "costing": costing,
        "directions_options": {"units": units}
    }

    try:
        response = requests.post(url, headers=headers, data=json.dumps(payload))
        response.raise_for_status()
        return response.json()
    except requests.RequestException as e:
        print(f"Error fetching route: {e}")
        return None

def decode(encoded):
    decoded = []
    previous = [0, 0]
    i = 0
    # for each byte
    while i < len(encoded):
        # for each coord (lat, lon)
        ll = [0, 0]
        for j in [0, 1]:
            shift = 0
            byte = 0x20
            # keep decoding bytes until you have this coord
            while byte >= 0x20:
                byte = ord(encoded[i]) - 63
                i += 1
                ll[j] |= (byte & 0x1f) << shift
                shift += 5
            # get the final value adding the previous offset and remember it for the next
            ll[j] = previous[j] + (~(ll[j] >> 1) if ll[j] & 1 else (ll[j] >> 1))
            previous[j] = ll[j]
        # scale by the precision and chop off long coords also flip the positions so
        # its the far more standard lon, lat instead of lat, lon
        decoded.append([float('%.6f' % (ll[1] * INV)), float('%.6f' % (ll[0] * INV))])
    # hand back the list of coordinates
    return decoded

def extract_shape_coordinates(route_data):
    """Extracts the shape polyline from the Valhalla response and decodes it into a list of (lat, lon) tuples."""
    shape_list = []
    try:
        if "trip" in route_data and "legs" in route_data["trip"]:
            for leg in route_data["trip"]["legs"]:
                if "shape" in leg:
                    shape_list.extend(decode(leg["shape"]))  # Decode polyline to (lat, lon)
    except Exception as e:
        print(f"Error extracting shape coordinates: {e}")
    
    return shape_list

def get_trace_attributes(url, shape_batch, units="miles"):
    """Sends a request to /trace_attributes API for a batch of points."""
    headers = {"Content-Type": "application/json"}
    payload = {
        "shape": [{"lat": round(float(lat), 6), "lon": round(float(lon), 6)} for lon, lat in shape_batch],  # Ensure correct format
        "costing": "auto",
        "shape_match": "map_snap",
        "directions_options": {"units": units}
    }

    try:
        response = requests.post(url, headers=headers, data=json.dumps(payload))
        response.raise_for_status()
        return response.json()
    except requests.exceptions.HTTPError as e:
        print(f"HTTP Error: {e}")
        print("Response content:", response.text)  # Debugging output
        return None
    except requests.RequestException as e:
        print(f"Error fetching trace attributes: {e}")
        return None

def extract_speed_data(trace_data, shape):
    """Maps speed limits to corresponding lat/lon points."""
    if not trace_data or "edges" not in trace_data:
        print("No speed data available.")
        return []

    speeds = []
    for edge in trace_data["edges"]:
        begin_index = edge.get("begin_shape_index", 0)
        end_index = edge.get("end_shape_index", 0)
        speed = edge.get("speed_limit", edge.get("speed", None))  # Speed limit fallback

        if speed is not None:
            for i in range(begin_index, end_index + 1):
                if i < len(shape):
                    speeds.append((*shape[i], speed))

    return speeds

def process_shape_in_batches(trace_url, shape):
    """Splits shape into batches, makes multiple requests, and merges results."""
    combined_speeds = []
    for i in range(0, len(shape), BATCH_SIZE):
        batch = shape[i:i + BATCH_SIZE]
        trace_data = get_trace_attributes(trace_url, batch)
        if trace_data:
            batch_speeds = extract_speed_data(trace_data, batch)
            combined_speeds.extend(batch_speeds)
    return combined_speeds

def save_to_file(data, filename):
    """Writes lat, lon, speed to a text file."""
    with open(filename, "w") as f:
        for lon, lat, speed in data:
            f.write(f"{lat}, {lon}, {speed}\n")
    print(f"Data saved to {filename}")

def generate_geojson_with_speed(lat_lon_speed, output_file="route.geojson"):
    """Generates a GeoJSON file with speed labels at points where speed changes."""

    # Extract coordinates and speeds
    shape_coordinates = [(lat, lon) for lat, lon, _ in lat_lon_speed]
    speeds = [speed for _, _, speed in lat_lon_speed]

    # LineString for the route
    line_feature = {
        "type": "Feature",
        "geometry": {
            "type": "LineString",
            "coordinates": [[lon, lat] for lon, lat in shape_coordinates]  # Swap lat/lon to lon/lat
        },
        "properties": {
            "name": "Generated Route",
            "stroke": "blue",
            "stroke-opacity": 0.6,
            "stroke-width": 3
        }
    }

    # Generate speed labels where speed changes
    speed_labels = []
    previous_speed = None

    for (lat, lon, speed) in lat_lon_speed:
        if speed != previous_speed:  # Add a label when speed changes
            speed_labels.append({
                "type": "Feature",
                "geometry": {
                    "type": "Point",
                    "coordinates": [lat, lon]  # Swap lat/lon
                },
                "properties": {
                    "name": f"Speed: {speed} mph",
                    "marker-symbol": "circle",  # Optional: for some viewers
                    "marker-color": "#FF0000"   # Red color for visibility
                }
            })
        previous_speed = speed

    # Combine all features
    geojson_data = {
        "type": "FeatureCollection",
        "features": [line_feature] + speed_labels  # Line + Speed markers
    }

    # Save to file
    with open(output_file, "w") as f:
        json.dump(geojson_data, f, indent=2)

def generate_geojson(lat_lon_speed, output_file="route.geojson"):
    """Generates a GeoJSON file with speed labels at points where speed changes."""

    # Extract coordinates and speeds
    shape_coordinates = [(lat, lon) for lat, lon, _ in lat_lon_speed]

    # LineString for the route
    line_feature = {
        "type": "Feature",
        "geometry": {
            "type": "LineString",
            "coordinates": [[lon, lat] for lon, lat in shape_coordinates]  # Swap lat/lon to lon/lat
        },
        "properties": {
            "name": "Generated Route",
            "stroke": "blue",
            "stroke-opacity": 0.6,
            "stroke-width": 3
        }
    }

    # Combine all features
    geojson_data = {
        "type": "FeatureCollection",
        "features": [line_feature]
    }

    # Save to file
    with open(output_file, "w") as f:
        json.dump(geojson_data, f, indent=2)

def main():
    """Main function with predefined variables."""
    route_url = "http://192.168.1.190:8002/route"  # Set your Valhalla server URL
    trace_url = "http://192.168.1.190:8002/trace_attributes"
    locations = [
        (47.6287, -122.3462),
        (33.7654, -118.1757)
    ]  # Set route points

    # Step 1: Get route data
    route_data = get_route(route_url, locations)
    if not route_data:
        print("No route data extracted.")
        return

    # Step 2: Extract shape coordinates from route
    shape = extract_shape_coordinates(route_data)
    if not shape:
        print("No shape data extracted.")
        return
    
    # Step 3: Get speed attributes for the extracted shape    
    lat_lon_speed = process_shape_in_batches(trace_url, shape)

    # Step 4: Save data to a file
    save_to_file(lat_lon_speed, "route.txt")

    # Step 5: Generate GeoJSON file
    generate_geojson(lat_lon_speed, "route.geojson")
    generate_geojson_with_speed(lat_lon_speed, "route_speed.geojson")

if __name__ == "__main__":
    main()
