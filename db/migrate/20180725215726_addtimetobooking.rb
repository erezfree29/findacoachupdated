class Addtimetobooking < ActiveRecord::Migration[5.2]
  def change
    add_column :bookings, :time, :time
  end
end
