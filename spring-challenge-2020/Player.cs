using System;
using System.Linq;
using System.IO;
using System.Text;
using System.Collections;
using System.Collections.Generic;

/*
    TODO:

    - Populate map with walls, pacDudes, and pellets
    - Add extra input parameters to PacMan and Pellet classes
    - Create class for Rock, Paper, Scissors types
    - Create comparator for Type:
        Paper beats Rock
        Rock beats Scissor
        Scissor beats Paper
    - Implement function to convert map to graph
    - Implement pathfinding algorithm to calculate optimal path.
        - What's the cost based on? 
*/

/**
 * Grab the pellets as fast as you can!
 **/
class Player
{
    static void Main(string[] args)
    {
        #region Map Initialization

        string[] inputs;
        inputs = Console.ReadLine().Split(' ');
        int width = int.Parse(inputs[0]); // size of the grid
        int height = int.Parse(inputs[1]); // top left corner is (x=0, y=0)

        /* contains the walls, pacDudes, and pellets in the game */
        //char[,] map = new char[width, height];

        for (int i = 0; i < height; i++)
        {
            string row = Console.ReadLine(); // one line of the grid: space " " is floor, pound "#" is wall
            // for (int j = 0; j < width; ++j) {
            //     map[j, i] = row[j];
            //     Console.Error.Write(row[j]);
            // }
            //Console.Error.WriteLine();
        }

        #endregion

        // game loop
        while (true)
        {
            #region Game RunTime Variables
            
            inputs = Console.ReadLine().Split(' ');
            int myScore = int.Parse(inputs[0]);
            int opponentScore = int.Parse(inputs[1]);
            int visiblePacCount = int.Parse(Console.ReadLine()); // all your pacs and enemy pacs in sight

            var pacDudes = new List<PacMan>();
            var pellets = new List<Pellet>();
            var commands = new StringBuilder();

            #endregion

            #region pacDudes Initialization

            for (int i = 0; i < visiblePacCount; i++)
            {
                inputs = Console.ReadLine().Split(' ');
                int pacId = int.Parse(inputs[0]); // pac number (unique within a team)
                bool mine = inputs[1] != "0"; // true if this pac is yours
                int x = int.Parse(inputs[2]); // position in the grid
                int y = int.Parse(inputs[3]); // position in the grid
                string typeId = inputs[4]; // unused in wood leagues
                int speedTurnsLeft = int.Parse(inputs[5]); // unused in wood leagues
                int abilityCooldown = int.Parse(inputs[6]); // unused in wood leagues

                pacDudes.Add(new PacMan(pacId, mine, new Position(x, y)));
            }

            #endregion

            int visiblePelletCount = int.Parse(Console.ReadLine()); // all pellets in sight
            for (int i = 0; i < visiblePelletCount; i++)
            {
                inputs = Console.ReadLine().Split(' ');
                int x = int.Parse(inputs[0]);
                int y = int.Parse(inputs[1]);
                int value = int.Parse(inputs[2]); // amount of points this pellet is worth
                pellets.Add(new Pellet(new Position(x, y), value));
            }

            foreach(var pacman in pacDudes) {
                if (!pacman.Mine) continue;

                Position pos = pacman.Position;
                int shortest_distance = 100;
                Position target = null;
                foreach(var pellet in pellets) {
                    int distance = pellet.Position.distance(pos);
                    shortest_distance = Math.Min(shortest_distance, distance);
                    if (distance == shortest_distance) {
                        target = pellet.Position;
                    }
                }

                string command = string.Format("MOVE {0} {1} {2}", pacman.PacId, target.x, target.y);
                if (commands.Length == 0) commands.Append(command);
                else commands.AppendFormat("|{0}", command);

            }

            Console.WriteLine(commands.ToString());
        }
    }

    #region Game Objects

    class PacMan {
        private int m_pacId;
        private bool m_mine;
        private Position m_position;

        public PacMan(int pacId, bool mine, Position position) {
            m_pacId = pacId;
            m_mine = mine;
            m_position = position;
        }
        public int PacId {
            get { return m_pacId; }
        }
        public bool Mine {
            get { return m_mine; }
        }
        public Position Position {
            get { return m_position; }
        }
    }

    class Pellet {
        private Position m_position;
        private int m_value;

        public Pellet(Position position, int value) {
            m_position = position;
            m_value = value;
        }
        public Position Position {
            get { return m_position; }
        }
        public int Value {
            get { return m_value; }
        }
    }

    // class MapPosition : Position {
    //     private int m_cost;
    //     public MapPosition(int x, int y) {
    //         m_x = x;
    //         m_y = y;
    //     }
    //     public MapPosition(int x, int y, int cost) {
    //         m_x = x;
    //         m_y = y;
    //         m_cost = m_cost;
    //     }
    //     public int Cost {
    //         get { return m_cost; }
    //         set { m_cost = value; }
    //     }
    // }
    class Position /*: IEquatable<Position> */{
        private int m_x;
        private int m_y;

        public Position(int x, int y) {
            m_x = x;
            m_y = y;
        }
        public int x {
            get { return m_x; }
        }
        public int y {
            get { return m_y; }
        }

        public int distance (Position final) {
            /* Manhattan distance */
            return Math.Abs(final.x - m_x) + Math.Abs(final.y - m_y);
        }

        /*public override bool Equals(object obj)
        {
            if (obj == null) return false;
            Part objAsPosition = obj as Position;
            if (objAsPosition == null) return false;
            else return Equals(objAsPosition);
        }

        public override bool Equals(Position position) {        
            return (m_x = position.x) 
                && (m_y == position.y);
        }

        public override int GetHashCode() => HashCode<int, int>.Combine(x, y);*/
    }

    #endregion

}
