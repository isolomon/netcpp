# netcpp
This is a cpp library which tries to simple your daily work and makes C++ a little easy to use. [TODO]

# StreamReader

        StreamReader r("/path/to/ini/file");

        while (true)
        {
            r.skipWhitespace();
            if (r.peek() < 0) break;
            if (r.peek() == '#') { r.moveToNextLine(); continue; }

            string section = r.readQuoted("[]");

            while (true) // read keys under this secsion
            {
                r.skipWhitespace();

                int first = r.peek();
                if (first == '[' || first < 0) break;
                
                if (first == '#' || first == ';' || first < 0)
                {
                    r.moveToNextLine();
                    continue;
                }

                string key = r.readToFirstOf("=");
                string value = r.readLine();
                
                // do something to store section/key/value
            }
        }
        
More samples about StreamReader you can see xml_reader.cpp or json.cpp, both of them are parsing with this StreamReader class.

#More
I will add more documents to explain how to use these library when I get time. If you like you can take a look classes like Buffer, delegate, XmlProfile. It is really fun if you know how to use them

