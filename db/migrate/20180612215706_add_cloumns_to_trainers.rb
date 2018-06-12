class AddCloumnsToTrainers < ActiveRecord::Migration[5.2]
  def change
    add_column :trainers, :first_name, :string
    add_column :trainers, :last_name, :string
    add_column :trainers, :age, :integer
    add_column :trainers, :gender, :string

  end
end
